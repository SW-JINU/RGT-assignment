from sqlalchemy import Column, Integer, String
from app.core.database import Base

class Book(Base):
    __tablename__ = "books"

    id = Column(Integer, primary_key=True, index=True)
    title = Column(String, nullable=False)
    author = Column(String, nullable=False)
    isbn = Column(String, unique=True, index=True, nullable=False)
    category = Column(String, index=True, nullable=False)
    total_copies = Column(Integer, nullable=False)
    available_copies = Column(Integer, nullable=False)
