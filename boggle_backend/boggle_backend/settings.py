from pathlib import Path
import os

BASE_DIR = Path(__file__).resolve().parent.parent

# Dev settings
SECRET_KEY = os.environ.get("DJANGO_SECRET_KEY", "dev-secret-not-for-prod")
DEBUG = True

# Allow your Codio box. For dev you can keep '*', or tighten later.
ALLOWED_HOSTS = ["*"]

# If CSRF bites you on POST, this helps on Codio:
CSRF_TRUSTED_ORIGINS = ["https://*.codio.io"]

INSTALLED_APPS = [
    "django.contrib.admin",
    "django.contrib.auth",
    "django.contrib.contenttypes",
    "django.contrib.sessions",
    "django.contrib.messages",
    "django.contrib.staticfiles",

    # third-party
    "corsheaders",

    # your app
    "gameapi",
]

MIDDLEWARE = [
    "corsheaders.middleware.CorsMiddleware",  # must be near the top
    "django.middleware.security.SecurityMiddleware",
    "django.contrib.sessions.middleware.SessionMiddleware",
    "django.middleware.common.CommonMiddleware",
    "django.middleware.csrf.CsrfViewMiddleware",
    "django.contrib.auth.middleware.AuthenticationMiddleware",
    "django.contrib.messages.middleware.MessageMiddleware",
    "django.middleware.clickjacking.XFrameOptionsMiddleware",
]

# Allow your React dev server to call Django
CORS_ALLOWED_ORIGINS = [
    "https://XXXXXXXXXXXXXXXX-3000.codio.io",  # <-- replace with YOUR React box URL
]
# (Optional during dev if domain is tricky)
# CORS_ALLOW_ALL_ORIGINS = True

ROOT_URLCONF = "boggle_backend.urls"

TEMPLATES = [
    {
        "BACKEND": "django.template.backends.django.DjangoTemplates",
        "DIRS": [],
        "APP_DIRS": True,
        "OPTIONS": {
            "context_processors": [
                "django.template.context_processors.debug",
                "django.template.context_processors.request",
                "django.contrib.auth.context_processors.auth",
                "django.contrib.messages.context_processors.messages",
            ],
        },
    },
]

WSGI_APPLICATION = "boggle_backend.wsgi.application"

DATABASES = {
    "default": {
        "ENGINE": "django.db.backends.sqlite3",
        "NAME": BASE_DIR / "db.sqlite3",
    }
}

AUTH_PASSWORD_VALIDATORS = []

LANGUAGE_CODE = "en-us"
TIME_ZONE = "UTC"
USE_I18N = True
USE_TZ = True

STATIC_URL = "static/"
DEFAULT_AUTO_FIELD = "django.db.models.BigAutoField"
