select category.category_id, category.name, count(film.film_id)
from category, film_category, film
where category.category_id = film_category.category_id and film.film_id = film_category.film_id
group by category.category_id
order by category.category_id 