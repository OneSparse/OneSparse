import sys
from dataclasses import dataclass

version = '0.1.0'
header_template = 'templates/header.sql'
scalar_func_template = 'templates/scalar_func.sql'
scalar_op_template = 'templates/scalar_op.sql'
scalar_cast_template = 'templates/scalar_cast.sql'
footer_template = 'templates/footer.sql'

@dataclass
class TypeSpec:
    name: str
    pgtype: str
    gbtype: str
    plus: str = '+'
    mult: str = '*'
    sub: str = '-'
    div: str = '/'

types = [
    TypeSpec('int64', 'bigint',   'GrB_INT64'),
    TypeSpec('int32', 'integer',  'GrB_INT32'),
    TypeSpec('int16', 'smallint', 'GrB_INT16'),
    TypeSpec('fp32',  'float4',   'GrB_INT32'),
    TypeSpec('fp64',  'float8',   'GrB_INT64'),
#    TypeSpec('bool',  'bool',             'GrB_BOOL', None, None, None, None),
]

def main(outfile):
    outfile.write(open(header_template, 'r').read())
    scalar_func = open(scalar_func_template, 'r').read()
    scalar_op = open(scalar_op_template, 'r').read()
    scalar_cast = open(scalar_cast_template, 'r').read()
    for t in types:
        outfile.write(scalar_func.format(type=t))
        outfile.write(scalar_op.format(type=t))
        outfile.write(scalar_cast.format(type=t))
    outfile.write(open(footer_template, 'r').read())

if __name__ == '__main__':
    outfile = sys.stdout if sys.argv[1] == '-' else open(sys.argv[1], 'w+')
    main(outfile)
