from sqlalchemy.orm import Session
from fastapi import HTTPException, status
from datetime import datetime
from typing import Optional  # ✅ 추가

from app.models.user import User
from app.models.book import Book
from app.models.loan import Loan
from app.core.security import hash_password, verify_password

def create_user(db: Session, username: str, email: str, password: str, full_name: str):
    if db.query(User).filter(User.username == username).first():
        raise HTTPException(status_code=409, detail="username already exists")
    if db.query(User).filter(User.email == email).first():
        raise HTTPException(status_code=409, detail="email already exists")

    user = User(
        username=username,
        email=email,
        password_hash=hash_password(password),
        full_name=full_name,
        role="USER"
    )
    db.add(user)
    db.commit()
    db.refresh(user)
    return user

def authenticate(db: Session, username: str, password: str) -> User:
    user = db.query(User).filter(User.username == username).first()
    if not user or not verify_password(password, user.password_hash):
        raise HTTPException(status_code=status.HTTP_401_UNAUTHORIZED, detail="Invalid credentials")
    return user

def create_book(db: Session, **kwargs):
    if db.query(Book).filter(Book.isbn == kwargs["isbn"]).first():
        raise HTTPException(status_code=409, detail="isbn already exists")
    total = kwargs["total_copies"]
    book = Book(**kwargs, available_copies=total)
    db.add(book)
    db.commit()
    db.refresh(book)
    return book

def search_books(db: Session, category: Optional[str], available: Optional[bool]):  # ✅ 수정
    q = db.query(Book)
    if category:
        q = q.filter(Book.category == category)
    if available is True:
        q = q.filter(Book.available_copies > 0)
    return q.all()

def delete_book(db: Session, book_id: int):
    book = db.query(Book).filter(Book.id == book_id).first()
    if not book:
        raise HTTPException(status_code=404, detail="book not found")
    db.delete(book)
    db.commit()

def borrow_book(db: Session, book_id: int, user_id: int):
    book = db.query(Book).filter(Book.id == book_id).with_for_update().first()
    if not book:
        raise HTTPException(status_code=404, detail="book not found")
    if book.available_copies <= 0:
        raise HTTPException(status_code=409, detail="no copies available")

    existing = db.query(Loan).filter(
        Loan.book_id == book_id,
        Loan.user_id == user_id,
        Loan.returned_at.is_(None)
    ).first()
    if existing:
        raise HTTPException(status_code=409, detail="already borrowed")

    loan = Loan(book_id=book_id, user_id=user_id)
    book.available_copies -= 1

    db.add(loan)
    db.commit()
    db.refresh(loan)
    return loan

def list_my_loans(db: Session, user_id: int):
    return db.query(Loan).filter(Loan.user_id == user_id).order_by(Loan.borrowed_at.desc()).all()

def return_loan(db: Session, loan_id: int, user_id: int):
    loan = db.query(Loan).filter(Loan.id == loan_id).first()
    if not loan:
        raise HTTPException(status_code=404, detail="loan not found")
    if loan.user_id != user_id:
        raise HTTPException(status_code=403, detail="not your loan")
    if loan.returned_at is not None:
        raise HTTPException(status_code=409, detail="already returned")

    book = db.query(Book).filter(Book.id == loan.book_id).first()
    loan.returned_at = datetime.utcnow()
    book.available_copies += 1

    db.commit()
    db.refresh(loan)
    return loan
