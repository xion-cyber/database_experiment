DELIMITER ;;
CREATE TRIGGER
`update_customer_history` AFTER UPDATE ON `customer_history` FOR EACH ROW BEGIN
	IF(old.store_id != new.store_id) OR (old.first_name != new.first_name) OR (old.last_name != new.last_name)
		THEN 
			UPDATE  customer_history
				SET
					store_id = new.store_id,
                    first_name = new.first_name,
                    last_name = new.last_name
			WHERE old.customer_id = new.customer_id;
	END IF;
	UPDATE	customer_history
		SET
			old.last_update = now()
		WHERE old.customer_id = new.customer_id;
END ;;
DELIMITER ;