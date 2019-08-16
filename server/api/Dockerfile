FROM python:latest
WORKDIR /app
COPY app.py .
COPY requirements.txt .
RUN pip install -r requirements.txt
RUN pip install gunicorn
EXPOSE 8000
CMD ["gunicorn", "-b", "0.0.0.0:8000", "app"]
