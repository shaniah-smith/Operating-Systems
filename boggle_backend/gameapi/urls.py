from django.urls import path
from . import views

urlpatterns = [
    # size is part of the path, e.g. /api/game_create/4/
    path('game_create/<int:size>/', views.game_create, name='game_create'),
]
