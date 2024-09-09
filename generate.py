import sys
from dataclasses import dataclass
from pathlib import Path

VERSION = '0.1.0'

@dataclass
class Template:
    name: str
    outfile: object = None
    version: str = VERSION
    debug: bool = False
    def path(self, part):
        return str(Path('templates', f'{self.name}_{part}.sql'))
    def test_path(self, part):
        return str(Path('sql', f'{self.name}_{part}.sql'))
    def file(self, part):
        return open(self.path(part), 'r')
    def read(self, part):
        return self.file(part).read()
    def write_part(self, part, **fmt):
        self.outfile.write(self.read(part).format(**fmt))

@dataclass
class Type:
    name: str
    short: str
    pgtype: str
    gbtype: str
    plus: str = '+'
    mult: str = '*'
    sub: str = '-'
    div: str = '/'


def write_source(outfile):

    types = [
        Type('int64', 'i8', 'bigint',   'GrB_INT64'),
        Type('int32', 'i4', 'integer',  'GrB_INT32'),
        Type('int16', 'i2', 'smallint', 'GrB_INT16'),
        Type('fp32',  'f4', 'float4',   'GrB_INT32'),
        Type('fp64',  'f8', 'float8',   'GrB_INT64'),
        Type('bool',  'b', 'bool',      'GrB_BOOL', None, None, None, None),
    ]

    objects = [
        Template('scalar', outfile=open(outfile, 'w+')),
        ]

    for o in objects:
        o.write_part('header')
        for t in types:
            o.write_part('func', type=t)
            o.write_part('cast', type=t)
            if t.plus is not None:
                o.write_part('math', type=t)
                o.write_part('op', type=t)
        o.write_part('footer')

if __name__ == '__main__':
    write_source(outfile=sys.stdout if sys.argv[1] == '-' else sys.argv[1])
