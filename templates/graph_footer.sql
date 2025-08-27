
create table user_defined_type (
    name text primary key,
    type_def text not null);

create table user_defined_unaryop (
    name text primary key,
    ztype text not null,
    xtype text not null,
    func text not null);

create table user_defined_indexunaryop (
    name text primary key,
    ztype text not null,
    xtype text not null,
    ttype text not null,
    func text not null);

create table user_defined_binaryop (
    name text primary key,
    ztype text not null,
    xtype text not null,
    ytype text not null,
    func text not null);

create table user_defined_indexbinaryop (
    name text primary key,
    ztype text not null,
    xtype text not null,
    ytype text not null,
    ttype text not null,
    func text not null);
