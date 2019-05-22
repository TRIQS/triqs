---
name: Bug report
about: Create a report to help us improve
title: Description
labels: bug

---

<!--
NOTE: The issue list is not the place for general questions regarding the usage of the code.
Please consider subscribing to the mailing list instead:
https://groups.google.com/a/flatironinstitute.org/forum/#!forum/triqs
-->

### Prerequisites

* [ ] Put an X between the brackets on this line if you have done all of the following:
    * Checked that your issue isn't already filed: https://github.com/issues?utf8=%E2%9C%93&q=is%3Aissue+user%3Atriqs

### Description

[Description of the issue]

### Steps to Reproduce

1. [First Step]
2. [Second Step]
3. [and so on...]

or paste a minimal code example reproduce the issue.

**Expected behavior:** [What you expect to happen]

**Actual behavior:** [What actually happens]

### Versions

You can get this information from copy and pasting the output of
```bash
python -c "from pytriqs.version import *; show_version(); show_git_hash();"
```
from the command line.

Please include the OS and what version of the OS you're running.

Further provide the output of the `cmake` and `make` command as well as the `CMakeCache.txt` file from the build directory using [gist](http://gist.github.com).

### Formatting

Please use markdown in your issue message. In order to show code correctly formatted, use a line with three backticks \`\`\` before and after source code.
For more markdown commands look at this useful [summary](https://guides.github.com/pdfs/markdown-cheatsheet-online.pdf).

### Additional Information

Any additional information, configuration or data that might be necessary to reproduce the issue.
