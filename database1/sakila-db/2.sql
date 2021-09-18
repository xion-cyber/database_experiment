select first_name, last_name
from actor, film, film_actor
where actor.actor_id = film_actor.actor_id and film.film_id = film_actor.film_id and film.title = "WEST LION"