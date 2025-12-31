import requests

base_url = "http://localhost:8000"

# 회원가입
signup_data = {
    "username": "john_doe",
    "email": "john@example.com",
    "password": "securepass123",
    "full_name": "John Doe"
}

signup_res = requests.post(f"{base_url}/auth/signup", json=signup_data)
print("SIGNUP:", signup_res.status_code, signup_res.text)

# 로그인
login_data = {
    "username": "john_doe",
    "password": "securepass123"
}

login_res = requests.post(f"{base_url}/auth/login", json=login_data)
print("LOGIN:", login_res.status_code, login_res.text)

token = login_res.json()["access_token"]
headers = {"Authorization": f"Bearer {token}"}

# 도서 등록
book_data = {
    "title": "Python Programming",
    "author": "John Smith",
    "isbn": "978-0123456789",
    "category": "Programming",
    "total_copies": 5
}

book_res = requests.post(
    f"{base_url}/books",
    json=book_data,
    headers=headers
)
print("CREATE BOOK:", book_res.status_code, book_res.text)

# 도서 조회
search_res = requests.get(
    f"{base_url}/books",
    params={"category": "Programming", "available": "true"}
)
print("SEARCH:", search_res.status_code, search_res.text)

# 도서 대출
borrow_data = {
    "book_id": 1,
    "user_id": 1
}

borrow_res = requests.post(
    f"{base_url}/loans",
    json=borrow_data,
    headers=headers
)
print("BORROW:", borrow_res.status_code, borrow_res.text)


# 내 대출 목록
loans_res = requests.get(
    f"{base_url}/users/me/loans",
    headers=headers
)
print("MY LOANS:", loans_res.status_code, loans_res.text)
