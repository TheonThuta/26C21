from sqlalchemy import create_engine
from sqlalchemy.orm import declarative_base

engine = create_engine('sqlite:///inspection_database.db')
Base = declarative_base()
