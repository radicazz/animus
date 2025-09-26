# Helipad Documentation

This directory contains all user-facing documentation for the Helipad engine, built with MkDocs.

## Structure

- **`handbook/`** - Core documentation organized by topic:
  - `index.md` - Homepage and getting started
  - `overview.md` - Engine overview and concepts
  - `styleguide.md` - Code and documentation style guidelines
  - `design/` - Architecture and design documents
  - `migration/` - Migration guides and upgrade notes

- **`api.md`** - API reference page (links to generated Doxygen docs)

- **`tooling.md`** - Developer tools and build system documentation

- **`assets/`** - Static assets for documentation:
  - `theme/simple-dark.css` - Custom Doxygen theme styling

## Building Documentation

Run the documentation generator from the project root:

```bash
uv run --extra documentation docs_gen
```

This generates both MkDocs and Doxygen outputs under `build/docs/generated/`.

For development, you can serve the MkDocs site locally:

```bash
mkdocs serve
```
