# Checklist Command Line Tool

A tool for managing simple checklists in the terminal.

A checklist is a sequence of sentences that must be verified to validate a
process.
A checklist *item* is one step of the sequence and it is composed by text.
An item can be *undone*, *checked* or *skipped*.
A checklist is *complete* when all the items are evaluated (checked or
skipped).

A *template* is a checklist that is used to make multiple *validation*s.
The template contains only items, meanwhile a validation contains both the
items and the answers.
An *answer* has a state (checked, skipped), a timestamp, a username, a note.
The note is mandatory in case of skipped state, optional in checked.

An item is identified by some text followed by the *marker* `!CHECK!`.
A template is than a text file that contains at least one marker.
It follows that a checklist can be completed in more than one run, using the
previously interrupted one are template for the next run.
The software will replace the marker with the answer.

*ATTENTION*: The checklist can be interrupted to complete a check, but it
cannot advance without an answer to respect the checklist order.

## Options

`--file, -f` The template file.

`--output, -o` The output file. Default: the input file.

`--user, -u` The username that appears in the answers. Default: the current
user.

## Workflow

Let's define a template for a generic software pre-release.

File: `release.ckl`
```
# ACME Prj v1.x

## There are no FIXME in the code.

!CHECK!

## The code was compiled both with and without the debug flag.

!CHECK!

## The CHANGELOG and VERSION is updated.

!CHECK!
```

In the example, the input and the title are formatted to form a markdown file,
but the application just replace the markers.

Run `checklist` on the template. The line starting with `>` are
prompts (on standard output) that wait the user input.

```
$checklist -f release.ckl -u "John Doe" -o v1.2.md
# ACME Prj v1.x

## There are no FIXME in the code.

>Checked?[y/n]: y
>Note: Verified using grep.

## The code was compiled both with and without the debug flag.

>Checked?[y/n]: y
>Note: 

## The CHANGELOG and VERSION is updated.

>Checked?[y/n]: n

```

The new file `v1.2.md` will be something like this

```
# ACME Prj v1.x

## There are no FIXME in the code.

Checked by John Doe at 2024-09-29T12:30:00.
Note: Verified using grep.

## The code was compiled both with and without the debug flag.

Checked by John Doe at 2024-09-29T12:30:05.

## The CHANGELOG and VERSION is updated.

!CHECK!
```

To finish the checklist, it can be used as input.

Since the input and output file are the same, the file is firstly completed
read and than over written.

```
$checklist -i v1.2.md -u "Jane Doe"
# ACME Prj v1.x

## There are no FIXME in the code.

Checked by John Doe at 2024-09-29T12:30:00.
Note: Verified using grep.

## The code was compiled both with and without the debug flag.

Checked by John Doe at 2024-09-29T12:30:05.

## The CHANGELOG and VERSION is updated.

>Checked?[y/n]: y
>Note: 

```

