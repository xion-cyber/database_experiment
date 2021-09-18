select film.film_id, title, sum(amount)
from film, inventory, rental, payment
where film.film_id = inventory.film_id and inventory.inventory_id = rental.inventory_id and rental.rental_id = payment.rental_id
group by film.film_id
order by sum(amount) DESC 
limit 1