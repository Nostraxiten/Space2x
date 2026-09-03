name: Bug report
description: Create a report to help us improve Space2X
labels: ["bug", "triage"]
body:
  - type: markdown
    attributes:
      value: |
        Thanks for taking the time to fill out this bug report!
  - type: dropdown
    id: os
    attributes:
      label: Operating System
      description: What platform are you running Space2X on?
      options:
        - Windows 11
        - Windows 10
        - Ubuntu 22.04 LTS / 24.04 LTS
        - Debian 12
        - Arch Linux
        - Fedora 39+
        - Other (please specify in description)
    validations:
      required: true
  - type: input
    id: version
    attributes:
      label: Space2X Version
      description: Which version or commit SHA of Space2X are you running?
      placeholder: e.g. v0.1.0 or commit 7a8b9c
    validations:
      required: true
  - type: textarea
    id: what-happened
    attributes:
      label: What happened?
      description: A clear and concise description of the bug.
    validations:
      required: true
  - type: textarea
    id: reproduction
    attributes:
      label: Steps to Reproduce
      description: Step by step guide to reproduce the issue.
      placeholder: |
        1. Open Space2X
        2. Navigate to 'Services'
        3. Click 'Start' on PostgreSQL
        4. See error
    validations:
      required: true
  - type: textarea
    id: logs
    attributes:
      label: Relevant Log Output
      description: Please attach relevant logs from the Space2X log viewer or CLI output.
      render: shell
