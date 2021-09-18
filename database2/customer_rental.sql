CREATE 
	ALGORITHM=UNDEFINED 
	DEFINER=`root`@`localhost` 
	SQL SECURITY DEFINER 
	VIEW `customer_rental` As 
	select 
		`c`.`customer_id` As `ID`, 
		concat(`c`.`first_name`,_utf8mb4' ',`c`.`last_name`) AS `name`,
		`film`.`title` As `title`,
		`r`.`rental_date` As `租借日期`,
		`r`.`return_date` As `归还日期`,
		(timestampdiff(day,`r`.`rental_date`,`r`.`return_date`)) As `租借时间`
	from 
		((`customer` `c` 
		join `rental` `r` on((`c`.`customer_id` = `r`.`customer_id` )) 
		join `inventory` on ((`r`.`inventory_id` = `inventory`.`inventory_id` )) 
		join film on ((`film`.`film_id` = `inventory`.`film_id`)) ))