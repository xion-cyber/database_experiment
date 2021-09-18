select store_id,address,district,city,country 
from address,city,country,store
where store.address_id = address.address_id and address.city_id=city.city_id and city.country_id=country.country_id
