from pydantic import BaseModel, Field
from datetime import datetime
from typing import Optional

class BorrowRequest(BaseModel):
    book_id: int = Field(gt=0)
    user_id: int = Field(gt=0)  # 과제 스펙대로 body에 받음

class LoanOut(BaseModel):
    id: int
    user_id: int
    book_id: int
    borrowed_at: datetime
    returned_at: Optional[datetime]

    class Config:
        from_attributes = True
