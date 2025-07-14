
CREATE OR REPLACE FUNCTION dnn(net matrix, bias matrix, image matrix, upto integer default 120)
RETURNS matrix AS $$
DECLARE
    i integer;
BEGIN
    for i in 1..upto loop
        image = image @ net;
        raise notice '%', info(image);
        image = image @++ bias;
    end loop;
    return image;
END;
$$ LANGUAGE plpgsql;
