import sys
from dataclasses import dataclass
from pathlib import Path

VERSION = '0.1.0'

@dataclass
class Template:
    name: str
    outfile: object = None
    test_dir: str = 'sql/'
    version: str = VERSION

    def path(self, part):
        return str(Path('templates', f'{self.name}_{part}.sql'))

    def file(self, part):
        return open(self.path(part), 'r')

    def read(self, part):
        return self.file(part).read()

    def write(self, part, **fmt):
        self.outfile.write(self.read(part).format(**fmt))

    def test_path(self, part):
        return str(Path('templates', f'test_{self.name}_{part}.sql'))

    def test_file(self, part):
        return open(self.test_path(part), 'r')

    def test_read(self, part):
        return self.test_file(part).read()

    def test_write(self, part, **fmt):
        typ = fmt.get('type')
        if typ is not None:
            outfile = open(Path(self.test_dir, f'test_{self.name}_{part}_{typ.short}.sql'), 'w+')
        else:
            outfile = open(Path(self.test_dir, f'test_{self.name}_{part}.sql'), 'w+')
        outfile.write(self.test_read(part).format(**fmt))

@dataclass
class Type:
    name: str
    short: str
    pgtype: str
    gbtype: str
    min: str = "-1"
    zero: str = "0"
    max: str = "1"
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
        Type('bool',  'b', 'bool',      'GrB_BOOL', 0, 0, 1, None, None, None, None),
    ]

    objects = [
        Template('scalar', outfile),
        ]

    for o in objects:
        o.write('header')
        o.test_write('header')
        for t in types:
            o.write('func', type=t)
            o.test_write('func', type=t)
            o.write('cast', type=t)
            o.test_write('cast', type=t)
            if t.plus is not None:
                o.write('math', type=t)
                o.write('op', type=t)
        o.write('footer')

if __name__ == '__main__':
    write_source(outfile=sys.stdout if sys.argv[1] == '-' else open(sys.argv[1], 'w+'))
