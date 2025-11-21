from django.http import JsonResponse
from django.views.decorators.http import require_GET
import random
import string

def _make_grid(size):
    letters = string.ascii_uppercase
    return [[random.choice(letters) for _ in range(size)] for _ in range(size)]

@require_GET
def game_create(request, size):
    grid = _make_grid(size)
    # React expects grid as a JSON-stringified 2D array (assignment uses single quotes then replaced)
    return JsonResponse({
        "name": "boggle",
        "size": size,
        "grid": str(grid),              # e.g. "[['A','B'],['C','D']]" (single quotes)
        "foundwords": "['TEST','WORD']" # simple demo list as string
    })

@require_GET
def games_list(request):
    # later you can return real saved games; placeholder for now
    return JsonResponse({"games": []})
