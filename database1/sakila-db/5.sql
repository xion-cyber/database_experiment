select actor.actor_id, first_name, last_name, count(film.film_id)
from film_actor, film, actor
where film_actor.film_id = film.film_id and film_actor.actor_id = actor.actor_id
group by film_actor.actor_id
having count(film.film_id) >= 40
