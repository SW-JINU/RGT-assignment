from fastapi import Depends, HTTPException, status
from sqlalchemy.orm import Session
from jose import jwt, JWTError
from app.core.database import SessionLocal
from app.core.config import JWT_SECRET_KEY, JWT_ALGORITHM
from app.models.user import User

def get_db():
    db = SessionLocal()
    try:
        yield db
    finally:
        db.close()

def get_current_user(db: Session = Depends(get_db), authorization: str = Depends(lambda: "")):
    # FastAPI에서 Header로 받는 정석 버전:
    # from fastapi import Header
    # def get_current_user(..., authorization: str | None = Header(default=None))
    # 여기선 main.py에서 Header로 주입하는 방식으로 처리할 예정
    raise NotImplementedError

def decode_token(token: str):
    try:
        return jwt.decode(token, JWT_SECRET_KEY, algorithms=[JWT_ALGORITHM])
    except JWTError:
        return None

def require_admin(user: User):
    if user.role != "ADMIN":
        raise HTTPException(status_code=status.HTTP_403_FORBIDDEN, detail="Admin only")
