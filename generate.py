import sys
from collections import defaultdict
from dataclasses import dataclass, field
from pathlib import Path

from pyclibrary import CLibrary, CParser

parser = CParser(['/usr/local/include/suitesparse/GraphBLAS.h'])
suitesparse = CLibrary('/usr/local/lib/libgraphblas.so', parser)

VERSION = '0.1.0'

def doctestify(test):
    lines = test.splitlines()
    markdown_lines = []
    in_code_block = False

    for line in lines:
        if line.startswith("--"):
            if in_code_block:
                markdown_lines.append("```")
                in_code_block = False
            markdown_lines.append(line[3:])
        else:
            if not in_code_block:
                markdown_lines.append("```")
                in_code_block = True
            markdown_lines.append(line)

    if in_code_block:
        markdown_lines.append("```")

    return "\n".join(markdown_lines)

@dataclass
class Template:
    name: str
    outfile: object = None
    context: defaultdict[dict] = field(default_factory=lambda: defaultdict(dict))
    test_dir: str = 'sql/'
    source_dir: str = 'src/'
    version: str = VERSION

    def sql_path(self, part):
        return Path('templates', f'{self.name}_{part}.sql')

    def sql_file(self, part):
        return open(self.sql_path(part), 'r')

    def sql_read(self, part):
        return self.sql_file(part).read()

    def sql_write(self, part):
        if self.sql_path(part).exists():
            self.outfile.write(self.sql_read(part).format(**self.context))

    def source_path(self, part):
        return Path('templates', f'{self.name}_{part}.h')

    def source_file(self, part):
        return open(self.source_path(part), 'r')

    def source_read(self, part):
        return self.source_file(part).read()

    def source_write(self, part):
        typ = self.context.get('type')
        if typ is not None:
            path = Path(self.source_dir, f'{self.name}/{self.name}_{part}_{typ.short}.h')
        else:
            path = Path(self.source_dir, f'{self.name}/{self.name}_{part}.h')

        if self.source_path(part).exists():
            outfile = open(path, 'w+')
            outfile.write(self.source_read(part).format(**self.context))

    def test_path(self, part):
        return Path('templates/tests', f'test_{self.name}_{part}.sql')

    def test_file(self, part):
        return open(self.test_path(part), 'r')

    def test_read(self, part):
        return self.test_file(part).read()

    def test_write(self, part):
        typ = self.context.get('type')
        if typ is not None:
            path = Path(self.test_dir, f'test_{self.name}_{part}_{typ.short}.sql')
        else:
            path = Path(self.test_dir, f'test_{self.name}_{part}.sql')

        if self.test_path(part).exists():
            outfile = open(path, 'w+')
            outfile.write(self.test_read(part).format(**self.context))

    def doc_write(self, part):
        typ = self.context.get('type')
        if typ is not None:
            path = Path(self.test_dir, f'test_{self.name}_{part}_{typ.short}.sql')
        else:
            path = Path(self.test_dir, f'test_{self.name}_{part}.sql')

        if self.test_path(part).exists():
            doc = Path('docs', *path.with_suffix('.md').parts[1:])
            outfile = open(doc, 'w+')
            outfile.write(doctestify(self.test_read(part).format(**self.context)))

    def write(self, part):
        self.sql_write(part)
        self.source_write(part)
        self.test_write(part)
        self.doc_write(part)

@dataclass
class Blog:
    file: Path
    test_dir: str = 'sql/'
    version: str = VERSION
    context: defaultdict[dict] = field(default_factory=lambda: defaultdict(dict))

    def write(self):
        doc = Path(self.test_dir, self.file.name)
        outfile = open(doc, 'w+')
        outfile.write(self.file.read_text().format(**self.context))

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

variables = suitesparse['variables'].items()
info = [i for i, j in variables if j[1].type_spec == 'GrB_Info']
types = [i for i, j in variables if j[1].type_spec == 'GrB_Type']
descriptors = [i for i, j in variables if j[1].type_spec == 'GrB_Descriptor']
unaries = [i for i, j in variables if j[1].type_spec == 'GrB_UnaryOp']
indexunaries = [i for i, j in variables if j[1].type_spec == 'GrB_IndexUnaryOp']
binops = [i for i, j in variables if j[1].type_spec == 'GrB_BinaryOp']
monoids = [i for i, j in variables if j[1].type_spec == 'GrB_Monoid']
semirings = [i for i, j in variables if j[1].type_spec == 'GrB_Semiring']

type_list = '\n'.join([
    f'-- | {s.lower()[4:]} | {s} |'
    for s in types])

type_decls = '\n'.join([
    f'\n    entry = typehash_insert(typehash, "{s.lower()[4:]}", &found);\n'
    f'    entry->name = strdup("{s.lower()[4:]}");\n'
    f'    entry->type = {s};'
    for s in types])

descriptor_list = '\n'.join([
    f'-- | {s.lower()[9:]} | {s} |'
    for s in descriptors])

descriptor_decls = '\n'.join([
    f'\n    entry = descriptorhash_insert(descriptorhash, "{s.lower()[9:]}", &found);\n'
    f'    entry->name = strdup("{s.lower()[9:]}");\n'
    f'    entry->descriptor = {s};'
    for s in descriptors])

unaryop_list = '\n'.join([
    f'-- | {s.lower()[4:]} | {s} |'
    for s in unaries])

unaryop_decls = '\n'.join([
    f'\n    entry = unaryophash_insert(unaryophash, "{s.lower()[4:]}", &found);\n'
    f'    entry->name = strdup("{s.lower()[4:]}");\n'
    f'    entry->unaryop = {s};'
    for s in unaries])

indexunaryop_list = '\n'.join([
    f'-- | {s.lower()[4:]} | {s} |'
    for s in indexunaries])

indexunaryop_decls = '\n'.join([
    f'\n    entry = indexunaryophash_insert(indexunaryophash, "{s.lower()[4:]}", &found);\n'
    f'    entry->name = strdup("{s.lower()[4:]}");\n'
    f'    entry->indexunaryop = {s};'
    for s in indexunaries])

binop_list = '\n'.join([
    f'-- | {s.lower()[4:]} | {s} |'
    for s in binops])

binop_decls = '\n'.join([
    f'\n    entry = binaryophash_insert(binaryophash, "{s.lower()[4:]}", &found);\n'
    f'    entry->name = strdup("{s.lower()[4:]}");\n'
    f'    entry->binaryop = {s};'
    for s in binops])

monoid_list = '\n'.join([
    f'-- | {s.lower()[4:]} | {s} |'
    for s in monoids])

monoid_decls = '\n'.join([
    f'\n    entry = monoidhash_insert(monoidhash, "{s.lower()[4:]}", &found);\n'
    f'    entry->name = strdup("{s.lower()[4:]}");\n'
    f'    entry->monoid = {s};'
    for s in monoids])

semiring_list = '\n'.join([
    f'-- | {s.lower()[4:]} | {s} |'
    for s in semirings])

semiring_decls = '\n'.join([
    f'\n    entry = semiringhash_insert(semiringhash, "{s.lower()[4:]}", &found);\n'
    f'    entry->name = strdup("{s.lower()[4:]}");\n'
    f'    entry->semiring = {s};'
    for s in semirings])

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
        Template('type', outfile, dict(decls=type_decls, names=type_list)),
        Template('descriptor', outfile, dict(decls=descriptor_decls, names=descriptor_list)),
        Template('unaryop', outfile, dict(decls=unaryop_decls, names=unaryop_list)),
        Template('indexunaryop', outfile, dict(decls=indexunaryop_decls, names=indexunaryop_list)),
        Template('binaryop', outfile, dict(decls=binop_decls, names=binop_list)),
        Template('monoid', outfile, dict(decls=monoid_decls, names=monoid_list)),
        Template('semiring', outfile, dict(decls=semiring_decls, names=semiring_list)),
        Template('scalar', outfile),
        Template('vector', outfile),
        Template('matrix', outfile),
        Template('graph', outfile),
        Template('examples', outfile),
        ]

    for o in objects:
        o.write('header')
        for t in types:
            o.context['type'] = t
            o.write('func')
            o.write('cast')
            if t.plus is not None:
                o.write('math')
                o.write('op')
        o.write('footer')

    for p in Path('templates/blog/posts').rglob('*.sql'):
        if p.is_file():
            b = Blog(p)
            b.write()

if __name__ == '__main__':
    write_source(outfile=sys.stdout if sys.argv[1] == '-' else open(sys.argv[1], 'w+'))
