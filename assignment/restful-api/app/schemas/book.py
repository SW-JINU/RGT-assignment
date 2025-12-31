from pydantic import BaseModel, Field

class BookCreate(BaseModel):
    title: str = Field(min_length=1)
    author: str = Field(min_length=1)
    isbn: str = Field(min_length=5)
    category: str = Field(min_length=1)
    total_copies: int = Field(ge=1)

class BookOut(BaseModel):
    id: int
    title: str
    author: str
    isbn: str
    category: str
    total_copies: int
    available_copies: int

    class Config:
        from_attributes = True
