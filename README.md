<!----------------------------------------------------------------------------
Copyright 2026 Munich Quantum Software Stack Project

Licensed under the Apache License, Version 2.0 with LLVM Exceptions (the
"License"); you may not use this file except in compliance with the License.
You may obtain a copy of the License at

https://github.com/Munich-Quantum-Software-Stack/MQSS-Client/blob/develop/LICENSE

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
License for the specific language governing permissions and limitations under
the License.

SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
-------------------------------------------------------------------------- -->

<p align="center">
  <picture>
    <source media="(prefers-color-scheme: dark)" srcset="https://raw.githubusercontent.com/Munich-Quantum-Software-Stack/QDMI/develop/docs/_static/mqss_logo_dark.svg" width="20%">
    <img src="https://raw.githubusercontent.com/Munich-Quantum-Software-Stack/QDMI/develop/docs/_static/mqss_logo.svg" width="20%">
  </picture>
</p>

# MQSS Client

<p align="center">
  <a href="https://munich-quantum-software-stack.github.io/MQSS-Client/">
  <img style="min-width: 200px !important; width: 30%;" src="https://img.shields.io/badge/documentation-blue?style=for-the-badge&logo=data:image/svg%2bxml;base64,PHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHZpZXdCb3g9IjAgMCA0NDggNTEyIj48IS0tIUZvbnQgQXdlc29tZSBGcmVlIDYuNi4wIGJ5IEBmb250YXdlc29tZSAtIGh0dHBzOi8vZm9udGF3ZXNvbWUuY29tIExpY2Vuc2UgLSBodHRwczovL2ZvbnRhd2Vzb21lLmNvbS9saWNlbnNlL2ZyZWUgQ29weXJpZ2h0IDIwMjQgRm9udGljb25zLCBJbmMuLS0+PHBhdGggZmlsbD0iI2ZmZmZmZiIgZD0iTTk2IDBDNDMgMCAwIDQzIDAgOTZMMCA0MTZjMCA1MyA0MyA5NiA5NiA5NmwyODggMCAzMiAwYzE3LjcgMCAzMi0xNC4zIDMyLTMycy0xNC4zLTMyLTMyLTMybDAtNjRjMTcuNyAwIDMyLTE0LjMgMzItMzJsMC0zMjBjMC0xNy43LTE0LjMtMzItMzItMzJMMzg0IDAgOTYgMHptMCAzODRsMjU2IDAgMCA2NEw5NiA0NDhjLTE3LjcgMC0zMi0xNC4zLTMyLTMyczE0LjMtMzIgMzItMzJ6bTMyLTI0MGMwLTguOCA3LjItMTYgMTYtMTZsMTkyIDBjOC44IDAgMTYgNy4yIDE2IDE2cy03LjIgMTYtMTYgMTZsLTE5MiAwYy04LjggMC0xNi03LjItMTYtMTZ6bTE2IDQ4bDE5MiAwYzguOCAwIDE2IDcuMiAxNiAxNnMtNy4yIDE2LTE2IDE2bC0xOTIgMGMtOC44IDAtMTYtNy4yLTE2LTE2czcuMi0xNiAxNi0xNnoiLz48L3N2Zz4=" alt="Documentation" />
  </a>
</p>
<!-- [DOXYGEN MAIN] -->

The **MQSS Client** is a unifying, context-aware access layer and programming library that separates
programming interfaces from the underlying compiler and runtime stacks. The **MQSS Client** is
integrated into the _Munich Quantum Software Stack (MQSS)_ to pass quantum jobs to the underlying
compiler infrastructure and query the properties of available quantum resources. It supports
multiple programming models, such as gate-based circuits and Hamiltonians. The **MQSS Client**
serves as an abstract layer between programming interfaces and the underlying compiler
infrastructure.

<!-- [DOXYGEN MAIN] -->

## FAQ

<!-- [DOXYGEN FAQ] -->

### What is MQSS?

_MQSS_ stands for _Munich Quantum Software Stack_ and is a project of the _Munich Quantum Valley_
initiative. It is jointly developed by the _Munich Quantum Valley (MQV) gGmbH_, _Leibniz
Supercomputing Centre (LRZ)_, the _Chair for Design Automation (CDA)_, and the _Chair of Computer
Architecture and Parallel Systems (CAPS)_ at TUM. It provides a comprehensive compilation and
runtime infrastructure for on-premise and remote quantum devices, support for modern compilation and
optimization techniques, and enables both current and future high-level abstractions for quantum
programming. This stack is designed to be capable of deployment in a variety of scenarios via
flexible configuration options. This includes stand-alone scenarios for individual systems, cloud
access to a variety of devices, as well as tight integration into HPC environments supporting
quantum acceleration. Concrete instances of the _MQSS_ are deployed at the LRZ and MQV gGmbH,
providing unified access to all of their quantum devices through multiple compatible access paths.
This includes a web portal, command line access via web credentials, as well as the option for
hybrid access with tight integration with HPC systems.It facilitates the connection between
end-users and quantum computing platforms by its integration within HPC infrastructures, such as
those found at the LRZ.

### What is the MQSS Client?

The **MQSS Client** acts as an abstraction layer for the front-end programming interfaces of _MQSS_.
Its purpose is to provide a unified interface for communication with the underlying middle-end.

### Where is the code?

The code is publicly available and hosted on GitHub at
[github.com/Munich-Quantum-Software-Stack/MQSS-Client](https://github.com/Munich-Quantum-Software-Stack/MQSS-Client).

### Under which license is MQSS Client released?

**MQSS Client** is released under the Apache License v2.0 with LLVM Exceptions. See
[LICENSE](https://github.com/Munich-Quantum-Software-Stack/MQSS-Client/blob/develop/LICENSE.md) for
more information. Any contribution to the project is assumed to be under the same license.

### In which languages ​​can the MQSS Client be used?

We use C++ to write the core of **MQSS Client**. With the provided Python bindings and C-API, **MQSS
Client** can be used with Python and C-based applications.

### How do I cite MQSS Client?

If you use MQSS Client in your research, please cite the following paper:

```bibtex
@misc{mqssclient,
      title={MQSS Client: Interface for Decoupling Quantum Programming Interfaces},
      author={Ercüment Kaya and Muhammad Nufail Farooqi and Minh Chung and Burak Mete and Martin Schulz and Jorge Echavarria},
      year={2026},
      eprint={2607.28563},
      archivePrefix={arXiv},
      primaryClass={cs.ET},
      url={https://arxiv.org/abs/2607.28563},
}
```

<!-- [DOXYGEN FAQ] -->

## 📬 Contact

The development of this project is led by the QCT department at the LRZ and the QSI department at
MQV gGmbH. You can also always reach us at
[mqss@munich-quantum-valley.de](mailto:mqss@munich-quantum-valley.de).

Please try to use the publicly accessible GitHub channels
([issues](https://github.com/Munich-Quantum-Software-Stack/MQSS-Client/issues),
[discussions](https://github.com/Munich-Quantum-Software-Stack/MQSS-Client/discussions),
[pull requests](https://github.com/Munich-Quantum-Software-Stack/MQSS-Client/pulls)) to allow for a
transparent and open discussion as much as possible.
