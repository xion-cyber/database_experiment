select first_name, last_name
from customer
where customer_id not in(
	select distinct customer.customer_id
    from customer,film, inventory, rental
    where film.film_id = inventory.film_id and inventory.inventory_id = rental.inventory_id
    and rental.customer_id = customer.customer_id and film.title = "WEST LION")