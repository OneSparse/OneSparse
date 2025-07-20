
CREATE OR REPLACE FUNCTION dnn(net matrix, bias matrix, image matrix, upto integer default 120)
RETURNS matrix AS $$
DECLARE
    i integer;
    mask matrix;
BEGIN
    for i in 1..upto loop
        image = image @ net;
        image = image @++ bias;
        image = choose(image, 'valuegt_fp32', 0);
        mask = choose(image, 'valuegt_fp32', 32);
        image = assign(image, 0, mask:=mask);
    end loop;
    return image;
END;
$$ LANGUAGE plpgsql;
