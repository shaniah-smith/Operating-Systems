from django.db import models

class Games(models.Model):
    name = models.CharField(max_length=50)
    size = models.IntegerField()
    grid = models.TextField()
    foundwords = models.TextField()

    def __str__(self):
        return f'{self.id} {self.name} ({self.size}x{self.size})'
