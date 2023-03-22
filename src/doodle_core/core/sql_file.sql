create table if not exists context
(
    id        integer primary key,
    com_hash  integer,
    json_data text
);
create index if not exists context_index_id on context (id);
create unique index if not exists ctx_hash_index on context (com_hash);
create table if not exists entity
(
    id          integer primary key,
    uuid_data   text,
    update_time datetime default CURRENT_TIMESTAMP not null
);
create index if not exists entity_index on entity (id);
create table if not exists com_entity
(
    id        integer
        primary key,
    entity_id integer,
    com_hash  integer,
    json_data text,
    foreign key (entity_id) references entity (id) on delete cascade on update cascade
);
create index if not exists com_entity_index on com_entity (id);
create index if not exists com_entity_index_hash on com_entity (com_hash);
create trigger if not exists UpdataLastTime_
    AFTER
        UPDATE OF json_data
    ON com_entity
begin
    update entity
    set update_time = CURRENT_TIMESTAMP
    where id = old.entity_id;
end;

create table if not exists doodle_info
(
    version_major integer not null,
    version_minor integer not null
);
create table if not exists usertab
(
    id               integer
        primary key,
    entity_id        integer,

    user_name        text             not null,
    permission_group bigint default 0 not null
);

create index if not exists usertab_id_index on usertab (id);

CREATE TABLE IF NOT EXISTS work_task_info
(
    id         integer
        primary key,
    entity_id  integer,

    user_id    text,
    task_name  text,
    region     text,
    abstract   text,
    time_point datetime,
    foreign key (entity_id) references entity (id) on delete cascade on update cascade
);
create index if not exists work_task_info_index on work_task_info (id);
create index if not exists work_task_info_index2 on work_task_info (entity_id);

CREATE TABLE IF NOT EXISTS episodes
(
    id        integer
        primary key,
    entity_id integer,

    eps       integer,
    foreign key (entity_id) references entity (id) on delete cascade on update cascade
);
create index if not exists episodes_index on episodes (id);
create index if not exists episodes_index2 on episodes (entity_id);

CREATE TABLE IF NOT EXISTS shot
(
    id        integer
        primary key,
    entity_id integer,

    shot_int  integer,
    shot_ab   text,
    foreign key (entity_id) references entity (id) on delete cascade on update cascade
);
create index if not exists shot_index on shot (id);
create index if not exists shot_index2 on shot (entity_id);




