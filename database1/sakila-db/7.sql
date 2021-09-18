select actor.first_name, actor.last_name
from film f1, film f2, film_actor a1, film_actor a2, actor
where f1.film_id = a1.film_id and f2.film_id = a2.film_id and a1.actor_id = a2.actor_id and actor.actor_id = a1.actor_id
		and f1.title = "FIRE WOLVES" and f2.title = "JAWBREAKER BROOKLYN"