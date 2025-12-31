from fastapi import FastAPI, Depends, Header, HTTPException
from sqlalchemy.orm import Session
from jose import jwt, JWTError
from typing import Optional, List  # ✅ 추가

from app.core.database import Base, engine
from app.core.config import JWT_SECRET_KEY, JWT_ALGORITHM
from app.core.security import create_access_token
from app.deps import get_db
from app import crud
from app.models.user import User
from app.schemas.auth import SignupRequest, LoginRequest, TokenResponse
from app.schemas.book import BookCreate, BookOut
from app.schemas.loan import BorrowRequest, LoanOut

Base.metadata.create_all(bind=engine)

app = FastAPI(title="Online Library Management API")

def get_current_user(
    db: Session = Depends(get_db),
    authorization: Optional[str] = Header(default=None)  # ✅ 수정
) -> User:
    if not authorization or not authorization.startswith("Bearer "):
        raise HTTPException(status_code=401, detail="Missing or invalid Authorization header")

    token = authorization.split(" ", 1)[1]
    try:
        payload = jwt.decode(token, JWT_SECRET_KEY, algorithms=[JWT_ALGORITHM])
        user_id = int(payload["sub"])
    except (JWTError, KeyError, ValueError):
        raise HTTPException(status_code=401, detail="Invalid token")

    user = db.query(User).filter(User.id == user_id).first()
    if not user:
        raise HTTPException(status_code=401, detail="User not found")
    return user

def require_admin(user: User = Depends(get_current_user)) -> User:
    if user.role != "ADMIN":
        raise HTTPException(status_code=403, detail="Admin only")
    return user

# ---------- Auth ----------
@app.post("/auth/signup")
def signup(req: SignupRequest, db: Session = Depends(get_db)):
    user = crud.create_user(db, req.username, req.email, req.password, req.full_name)
    return {"id": user.id, "username": user.username, "email": user.email, "full_name": user.full_name}

@app.post("/auth/login", response_model=TokenResponse)
def login(req: LoginRequest, db: Session = Depends(get_db)):
    user = crud.authenticate(db, req.username, req.password)
    token = create_access_token(subject=str(user.id), role=user.role)
    return TokenResponse(access_token=token)

# ---------- Books ----------
@app.post("/books", response_model=BookOut)
def create_book(req: BookCreate, db: Session = Depends(get_db), _: User = Depends(require_admin)):
    return crud.create_book(db, **req.model_dump())

@app.get("/books", response_model=List[BookOut])  # ✅ 수정
def get_books(
    category: Optional[str] = None,  # ✅ 수정
    available: Optional[bool] = None,  # ✅ 수정
    db: Session = Depends(get_db)
):
    return crud.search_books(db, category, available)

@app.delete("/books/{book_id}", status_code=204)
def delete_book(book_id: int, db: Session = Depends(get_db), _: User = Depends(require_admin)):
    crud.delete_book(db, book_id)
    return None

# ---------- Loans ----------
@app.post("/loans", response_model=LoanOut)
def borrow(req: BorrowRequest, db: Session = Depends(get_db), user: User = Depends(get_current_user)):
    if req.user_id != user.id:
        raise HTTPException(status_code=403, detail="user_id does not match token user")
    return crud.borrow_book(db, req.book_id, req.user_id)

@app.get("/users/me/loans", response_model=List[LoanOut])  # ✅ 수정
def my_loans(db: Session = Depends(get_db), user: User = Depends(get_current_user)):
    return crud.list_my_loans(db, user.id)

@app.post("/loans/{loan_id}/return", response_model=LoanOut)
def return_book(loan_id: int, db: Session = Depends(get_db), user: User = Depends(get_current_user)):
    return crud.return_loan(db, loan_id, user.id)

@app.delete("/loans/{loan_id}", response_model=LoanOut)
def delete_loan_as_return(loan_id: int, db: Session = Depends(get_db), user: User = Depends(get_current_user)):
    return crud.return_loan(db, loan_id, user.id)
