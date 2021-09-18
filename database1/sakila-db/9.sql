select  distinct film.title
from film, inventory i1, inventory i2, store s1, store s2
where film.film_id = i1.film_id and film.film_id = i2.film_id and s1.store_id = i1.store_id
		and s2.store_id = i2.store_id and s1.store_id <> s2.store_id