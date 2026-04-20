# Quotations

A Qt-based desktop application to manage a collection of quotations and use them to automatically update an e-mail signature file.

## Intent

Many e-mail clients support a plain-text signature file that is appended to every outgoing message. This tool lets you maintain a library of quotations (text, author, optional book reference) stored in an XML file, randomly pick one, and write it into a signature template — producing the final signature file that your e-mail client reads.

Typical workflow:

1. Configure the three file paths in the **Settings** tab:
   - **Template file** — the signature skeleton containing `{quote}`, `{author}`, and `{book}` placeholders.
   - **Signature file** — the output file your e-mail client reads.
   - **Quotations file** — the XML library of quotations.
2. Click **Another quotation** to cycle through your library at random.
3. Click **Update signature** to write the selected quotation into the template and save the result to the signature file.

Advanced mode (checkbox) additionally lets you add, edit, and remove quotations, and save the updated XML library.

## Picture display

The application can display a random image alongside the current quotation. In the **Settings** tab, set the **Pictures directory** field to a folder containing image files. Every time **New image** is clicked (or a new quotation is loaded), a random image from that folder — including sub-folders — is picked and shown scaled to fit the view (max 600 × 400 px).

Supported formats are whatever Qt's image reader supports on your platform (PNG, JPEG, GIF, BMP, etc.). Leave the field empty to disable the feature.

## Example files

The `example/` directory contains ready-to-use sample files:

| File | Description |
|------|-------------|
| `example/signatures.xml` | Sample quotations library with 10 entries in the expected XML format. |
| `example/signature_template.txt` | Sample signature template showing how to place the `{quote}`, `{author}`, and `{book}` placeholders. |

### Quotations XML format

```xml
<?xml version="1.0" encoding="UTF-8"?>
<citations>
    <citation>
        <text>The quotation text.</text>
        <author>Author Name</author>
        <book>Optional Book Title</book>  <!-- omit if not applicable -->
    </citation>
    ...
</citations>
```

### Signature template format

Any plain-text file containing one or more of the following placeholders:

| Placeholder | Replaced with |
|-------------|---------------|
| `{quote}`  | The quotation text |
| `{author}` | The author's name |
| `{book}`   | `in <title>` when a book is set, empty string otherwise |

## Dependencies

- Qt 5 or Qt 6 (modules: `core`, `gui`, `widgets`, `xml`)
- A C++11-capable compiler (GCC, Clang, MSVC)
- `qmake` (comes with Qt)

## Compiling

```bash
# 1. Generate the Makefile
qmake quotations.pro

# 2. Build
make

# 3. Run
./quotations
```

On Windows, replace `make` with `nmake` (MSVC) or `mingw32-make` (MinGW).

### Optional: update translations

```bash
lupdate quotations.pro   # extract translatable strings into quotes_fr.ts
lrelease quotes_fr.ts    # compile to quotes_fr.qm
```

Place the resulting `.qm` file alongside the binary so Qt can load it automatically based on the system locale.

## License

This project is distributed under the **Apache License 2.0**. See the [LICENSE](LICENSE) file for the full terms.
In short, you are free to use, modify, and distribute this software, provided you retain the original copyright and license notice.

