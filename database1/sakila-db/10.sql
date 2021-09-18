select first_name, last_name,timestampdiff(microsecond,rental.rental_date,rental.return_date)
from rental, customer
where rental.customer_id = customer.customer_id
order by(timestampdiff(microsecond,rental.rental_date,rental.return_date)) DESC
limit 3
