/*
 * Copyright 2024 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/***********************************************************************************/
/* This file is automatically generated using bindtool and can be manually
 * edited  */
/* The following lines can be configured to regenerate this file during cmake */
/* If manual edits are made, the following tags should be modified accordingly.
 */
/* BINDTOOL_GEN_AUTOMATIC(0) */
/* BINDTOOL_USE_PYGCCXML(0) */
/* BINDTOOL_HEADER_FILE(thirtytwobit_source.h) */
/* BINDTOOL_HEADER_FILE_HASH(a64021eea51c57b6badddc756766a947) */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/hnchiio/thirtytwobit_source.h>
// pydoc.h is automatically generated in the build directory
#include <thirtytwobit_source_pydoc.h>

void bind_thirtytwobit_source(py::module &m) {

  using thirtytwobit_source = gr::hnchiio::thirtytwobit_source;

  py::class_<thirtytwobit_source, gr::block, gr::basic_block,
             std::shared_ptr<thirtytwobit_source>>(m, "thirtytwobit_source",
                                                   D(thirtytwobit_source))

      .def(py::init(&thirtytwobit_source::make), py::arg("uri"),
           py::arg("device"), py::arg("channels"), py::arg("device_phy"),
           py::arg("params"), py::arg("buffer_size") = DEFAULT_BUFFER_SIZE,
           py::arg("decimation") = 0, D(thirtytwobit_source, make))

      ;
}
