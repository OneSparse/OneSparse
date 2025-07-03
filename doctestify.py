import sys
from collections import defaultdict
from dataclasses import dataclass, field
from pathlib import Path

VERSION = '0.1.0'

def doctestify(test):
    lines = test.splitlines()
    markdown_lines = []
    in_code_block = False

    for line in lines:
        if line.startswith("\\") or "-- pragma:hide" in line:
            continue
        if line.startswith("--") and not line.startswith("---"):
            if in_code_block:
                markdown_lines.append("```")
                in_code_block = False
            markdown_lines.append(line[3:])
        else:
            if not in_code_block:
                markdown_lines.append("``` postgres-console")
                in_code_block = True
            line = line.replace("\u21B5", " ")
            markdown_lines.append(line)

    if in_code_block:
        markdown_lines.append("```")

    return "\n".join(markdown_lines)

@dataclass
class Template:

    name: str
    dest: str = 'docs'
    context: defaultdict[dict] = field(default_factory=lambda: defaultdict(dict))
    expected_dir: str = 'expected/'
    version: str = VERSION

    def write(self, part):
        typ = self.context.get('type')
        if typ is not None:
            test = Path(self.expected_dir, f'{self.name}_{part}_{typ.short}.out')
        else:
            test = Path(self.expected_dir, f'{self.name}_{part}.out')

        if test.exists():
            infile = open(test, 'r')
            doc = Path(self.dest, *test.with_suffix('.md').parts[1:])
            outfile = open(doc, 'w+')
            outfile.write(doctestify(infile.read()))

@dataclass
class Blog:

    file: Path
    dest: str = 'docs/blog/posts'
    expected_dir: str = 'expected/'
    version: str = VERSION

    def write(self):
        post = Path(self.expected_dir, self.file.with_suffix('.out').name)
        doc = Path(self.dest, self.file.with_suffix('.md').name)
        outfile = open(doc, 'w+')
        outfile.write(doctestify(post.read_text()))

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


def write_docs():

    types = [
        Type('int64', 'i8', 'bigint',   'GrB_INT64'),
        Type('int32', 'i4', 'integer',  'GrB_INT32'),
        Type('int16', 'i2', 'smallint', 'GrB_INT16'),
        Type('fp32',  'f4', 'float4',   'GrB_INT32'),
        Type('fp64',  'f8', 'float8',   'GrB_INT64'),
        Type('bool',  'b', 'bool',      'GrB_BOOL', 0, 0, 1, None, None, None, None),
    ]

    objects = [
        Template('test_type'),
        Template('test_descriptor'),
        Template('test_unaryop'),
        Template('test_indexunaryop'),
        Template('test_binaryop'),
        Template('test_monoid'),
        Template('test_semiring'),
        Template('test_scalar'),
        Template('test_vector'),
        Template('test_matrix'),
        Template('test_examples'),
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

def write_blog():
    for p in Path('templates/blog/posts').rglob('*.sql'):
        if p.is_file():
            b = Blog(p)
            b.write()

if __name__ == '__main__':
    write_docs()
    write_blog()
