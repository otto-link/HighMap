/*
 @licstart  The following is the entire license notice for the JavaScript code in this file.

 The MIT License (MIT)

 Copyright (C) 1997-2020 by Dimitri van Heesch

 Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 and associated documentation files (the "Software"), to deal in the Software without restriction,
 including without limitation the rights to use, copy, modify, merge, publish, distribute,
 sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all copies or
 substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 @licend  The above is the entire license notice for the JavaScript code in this file
*/
var NAVTREE =
[
  [ "HighMap library (C++)", "index.html", [
    [ "Architecture Overview", "md_docs_2content_2dev_2architecture.html", [
      [ "1. Module Hierarchy", "md_docs_2content_2dev_2architecture.html#autotoc_md2", null ],
      [ "2. Layers", "md_docs_2content_2dev_2architecture.html#autotoc_md3", [
        [ "Foundational Math", "md_docs_2content_2dev_2architecture.html#autotoc_md4", null ],
        [ "Core Data Structures", "md_docs_2content_2dev_2architecture.html#autotoc_md5", null ],
        [ "Array Processing", "md_docs_2content_2dev_2architecture.html#autotoc_md6", null ],
        [ "Domain Simulations", "md_docs_2content_2dev_2architecture.html#autotoc_md7", null ],
        [ "Export & I/O", "md_docs_2content_2dev_2architecture.html#autotoc_md8", null ]
      ] ],
      [ "3. Design Principles", "md_docs_2content_2dev_2architecture.html#autotoc_md9", [
        [ "Contiguous Storage and Explicit Memory Control", "md_docs_2content_2dev_2architecture.html#autotoc_md10", null ],
        [ "Separation of Storage and Algorithm", "md_docs_2content_2dev_2architecture.html#autotoc_md11", null ],
        [ "Stateless Processing Functions", "md_docs_2content_2dev_2architecture.html#autotoc_md12", null ],
        [ "Deterministic Seeding", "md_docs_2content_2dev_2architecture.html#autotoc_md13", null ]
      ] ],
      [ "4. Subsystem Deep Dives", "md_docs_2content_2dev_2architecture.html#autotoc_md14", null ],
      [ "5. Investigations and Benchmarks", "md_docs_2content_2dev_2architecture.html#autotoc_md15", null ]
    ] ],
    [ "FFT vs. GPU Convolution and Morphological Expansion", "md_docs_2content_2dev_2investigations_2fft__convolution__and__morphology.html", [
      [ "1. Context and Motivation", "md_docs_2content_2dev_2investigations_2fft__convolution__and__morphology.html#autotoc_md17", null ]
    ] ],
    [ "Scatter Architecture: ScatterField, Forest, and RockField", "md_docs_2content_2dev_2scatter__field.html", [
      [ "1. Class and Dependency Diagram", "md_docs_2content_2dev_2scatter__field.html#autotoc_md24", null ],
      [ "2. Design Principles", "md_docs_2content_2dev_2scatter__field.html#autotoc_md25", [
        [ "Uniform Instance Representation", "md_docs_2content_2dev_2scatter__field.html#autotoc_md26", null ],
        [ "Inheritance and Reuse", "md_docs_2content_2dev_2scatter__field.html#autotoc_md27", null ],
        [ "Descriptor Separation", "md_docs_2content_2dev_2scatter__field.html#autotoc_md28", null ]
      ] ],
      [ "3. Subsystem Connections", "md_docs_2content_2dev_2scatter__field.html#autotoc_md29", [
        [ "Flora Subsystem (<tt>Forest</tt>)", "md_docs_2content_2dev_2scatter__field.html#autotoc_md30", null ],
        [ "Rocks Subsystem (<tt>RockField</tt>)", "md_docs_2content_2dev_2scatter__field.html#autotoc_md31", null ]
      ] ],
      [ "4. Converting to Raster Fields", "md_docs_2content_2dev_2scatter__field.html#autotoc_md32", null ]
    ] ],
    [ "Core Concepts", "md_docs_2content_2getting__started_2core__concepts.html", [
      [ "1. Elevation Representations", "md_docs_2content_2getting__started_2core__concepts.html#autotoc_md34", [
        [ "Array", "md_docs_2content_2getting__started_2core__concepts.html#autotoc_md35", null ]
      ] ],
      [ "2. Bivariate Functions", "md_docs_2content_2getting__started_2core__concepts.html#autotoc_md36", null ],
      [ "3. Geometric Primitives", "md_docs_2content_2getting__started_2core__concepts.html#autotoc_md37", null ],
      [ "4. Scattering", "md_docs_2content_2getting__started_2core__concepts.html#autotoc_md38", null ]
    ] ],
    [ "Quickstart", "md_docs_2content_2getting__started_2quickstart.html", [
      [ "1. Example", "md_docs_2content_2getting__started_2quickstart.html#autotoc_md40", null ],
      [ "2. CMake Integration", "md_docs_2content_2getting__started_2quickstart.html#autotoc_md41", null ],
      [ "3. Basic Operations", "md_docs_2content_2getting__started_2quickstart.html#autotoc_md42", null ],
      [ "4. Next Steps", "md_docs_2content_2getting__started_2quickstart.html#autotoc_md43", null ]
    ] ],
    [ "HighMap Documentation", "md_docs_2index.html", [
      [ "Documentation", "md_docs_2index.html#autotoc_md45", [
        [ "Getting Started", "md_docs_2index.html#autotoc_md46", null ],
        [ "Developer Documentation", "md_docs_2index.html#autotoc_md47", null ]
      ] ],
      [ "Installation", "md_docs_2index.html#autotoc_md48", null ]
    ] ],
    [ "Optional Tracy CPU profiling", "md_docs_2profiling.html", null ],
    [ "Bibliography", "citelist.html", null ],
    [ "Namespaces", "namespaces.html", [
      [ "Namespace List", "namespaces.html", "namespaces_dup" ],
      [ "Namespace Members", "namespacemembers.html", [
        [ "All", "namespacemembers.html", "namespacemembers_dup" ],
        [ "Functions", "namespacemembers_func.html", "namespacemembers_func" ],
        [ "Variables", "namespacemembers_vars.html", null ],
        [ "Typedefs", "namespacemembers_type.html", null ],
        [ "Enumerations", "namespacemembers_enum.html", null ],
        [ "Enumerator", "namespacemembers_eval.html", "namespacemembers_eval" ]
      ] ]
    ] ],
    [ "Classes", "annotated.html", [
      [ "Class List", "annotated.html", "annotated_dup" ],
      [ "Class Index", "classes.html", null ],
      [ "Class Hierarchy", "hierarchy.html", "hierarchy" ],
      [ "Class Members", "functions.html", [
        [ "All", "functions.html", "functions_dup" ],
        [ "Functions", "functions_func.html", "functions_func" ],
        [ "Variables", "functions_vars.html", "functions_vars" ],
        [ "Typedefs", "functions_type.html", null ],
        [ "Enumerations", "functions_enum.html", null ],
        [ "Related Symbols", "functions_rela.html", null ]
      ] ]
    ] ],
    [ "Files", "files.html", [
      [ "File List", "files.html", "files_dup" ],
      [ "File Members", "globals.html", [
        [ "All", "globals.html", "globals_dup" ],
        [ "Functions", "globals_func.html", "globals_func" ],
        [ "Variables", "globals_vars.html", null ]
      ] ]
    ] ],
    [ "Examples", "examples.html", "examples" ]
  ] ]
];

var NAVTREEINDEX =
[
"add__noise_8cpp.html",
"classhmap_1_1Array.html#a0240a827bf1e8e52f4448746d244ea31",
"classhmap_1_1BigTiffWriter.html#ae4cc55e3fa4015a5bb35205d137cba4d",
"classhmap_1_1CoordFrame.html#aebce63606ddca495d41de463cafb33bd",
"classhmap_1_1FieldFunction.html#acfaece8d19fe130f277072fb82b3b673",
"classhmap_1_1KDTree.html#a957d4e8b3845faf98b017d6fa0eee2b5",
"classhmap_1_1PdfSampler.html#ae36e2260565931cbb6a9f6b1c8357fd0",
"classhmap_1_1ScatterField.html#ad4986df1aa3dff53929c88f0d5b26d33",
"classhmap_1_1TileStorage.html#a2feeb317dfb3fa8057fcd4daf4202fd8",
"coherent__noise_8hpp.html#a10ace74a842f9d2dc1f15dde91263a40",
"deposition__fill__holes_8cpp.html#a67fee4e11fb0b003d0ebb091e85533fe",
"export__tiled_8cpp.html#a91708f107a53c4b8ef68bb1d0291c73f",
"filters_8hpp.html#aeb2e79274db3110eaa716f2472399bff",
"functions_~.html",
"hydraulic__mise_8cpp.html#aa5eba7d9e64211ca66cef16db4723475",
"island_8cpp.html#a7aac37734b22ffed4807f274511861be",
"math_2array_8hpp.html#ad1eb8e81ef31a0e0bc7a5bd093c3aefc",
"namespacehmap.html#a072f0c93c6e3e5e0f8f88c760850dce8",
"namespacehmap.html#a3957ac4ae673f8ad402b5dd71e99b015",
"namespacehmap.html#a738bcdb6b3493bf7f2cbfcfc03a00159",
"namespacehmap.html#ab4d8048b79080012b8e60c270de8137f",
"namespacehmap.html#add641a7fc26ae08b1f679cbb7dd43177",
"namespacehmap_1_1gpu.html#a72c9d220589d2724b4e48e9c37ff55ad",
"namespacemembers_func_b.html",
"point__sampling_8cpp.html#a9cd51142544036bc8ce9c679547e9c65",
"range_8hpp.html#abc5d54d0d76a86f083db977837c28d69",
"signed__curvature__from__distance_8cpp.html#ad4ab7bb38b3ea37fa912190251c1ed76",
"structhmap_1_1GaussianPush.html#a01d24798bc375d4b997689c1a48447f4",
"structhmap_1_1RockDistribution.html#a159ba0c156af8943c9b0f4313c57e48c",
"structhmap_1_1TerrainTriMesh_1_1Triangle.html",
"terrain__super__resolution_8cpp.html#a580605e460b9b7573340233906b67517",
"test__fill__talus_8cpp.html",
"test__math_8cpp.html#ad407fc2f663f38212b448afd60501371",
"test__select__angle_8cpp.html#a501e1519bfad1340f29d1f36a84c3820",
"transform_8hpp.html#a19dec938e688ef4b73cf27ff3934043b"
];

var SYNCONMSG = 'click to disable panel synchronisation';
var SYNCOFFMSG = 'click to enable panel synchronisation';