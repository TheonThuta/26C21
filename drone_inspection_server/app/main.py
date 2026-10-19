from fastapi import FastAPI, Depends
from sqlalchemy.orm import Session
from .database import get_db
from . import models

app = FastAPI()

@app.get('/health')
def server_health():
    return {'status' : 'ok'}

@app.get('/missions')
def list_missions(db: Session = Depends(get_db)):
    return db.query(models.Mission).all()