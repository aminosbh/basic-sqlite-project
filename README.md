# Basic SQLite project

This project is a basic C [SQLite][] project that uses [CMake][] as a build system.


## Overview

This project demonstrates the usage of the SQLite C library. It, first, tries
to open the `users.db` SQLite database, and creates a new one if it does not
yet exists. Then, it creates a `user` table, if it is not already created:

```sql
CREATE TABLE `user` (
    `id`            INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE,
    `first_name`    TEXT NOT NULL,
    `last_name`     TEXT NOT NULL,
    `birthday`      INTEGER,
    `email`         TEXT,
    `password`      TEXT
);
```

After that, it will prompt you to enter the number of users to be added (zero
could be specified). If a positive value is entered, you should specify all
details for each user.

Finally, it will display a table showing the full list of users, but just the
columns `first_name`, `last_name` and `birthday`.

**SQLite documentation:**

- [Doc](https://www.sqlite.org/docs.html)
- [Quick start](https://www.sqlite.org/quickstart.html)
- [Introduction to the SQLite C/C++ APIs](https://www.sqlite.org/cintro.html)
- [C/C++ API reference](https://www.sqlite.org/c3ref/intro.html)

**GUI editor for SQLite databases:**

[DB Brwoser for SQLite][QSLite browser] is recommended for those who would like
to edit their databases and verify that their code is working like they expect.

## Dependencies

- [Git][]
- C Compiler (gcc, ...)
- [CMake][]
- [SQLite][] library

**On Debian/Ubuntu based distributions, use the following command:**

```sh
sudo apt install git build-essential pkg-config cmake cmake-data libsqlite3-dev
```

## Build instructions

```sh
# Clone this repo
git clone https://gitlab.com/aminosbh/basic-sqlite-project.git
cd basic-sqlite-project

# Create a build folder
mkdir build
cd build

# Build
cmake ..
make

# Run
./basic-sqlite-project
```

### Open the project with an IDE under Linux

See [IDE_USAGE.md](IDE_USAGE.md) for details.

## License

Author: Amine B. Hassouna [@aminosbh](https://gitlab.com/aminosbh)

This project is distributed under the terms of the MIT license
[&lt;LICENSE&gt;](LICENSE).



[SQLite]: https://www.sqlite.org
[CMake]: https://cmake.org
[Git]: https://git-scm.com
[QSLite browser]: https://sqlitebrowser.org
