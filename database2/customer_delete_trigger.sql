DELIMITER ;;
CREATE TRIGGER
`delete_customer` AFTER DELETE ON `customer` FOR EACH ROW BEGIN
	INSERT INTO customer_history(customer_id, store_id, first_name, last_name, email, address_id,active,delete_date,
        last_update)
		VALUES(old.customer_id, old.store_id, old.first_name, old.last_name, old.email, old.address_id,old.active,now(),
        old.last_update);
END ;;
DELIMITER ;

