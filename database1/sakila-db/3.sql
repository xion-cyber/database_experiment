select first_name, last_name, sum(amount)
from payment, customer
where payment.customer_id = customer.customer_id
group by customer.customer_id
order by sum(amount) DESC limit 3
