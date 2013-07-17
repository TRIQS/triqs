Cannot find source file "archive_exception.cpp"
===============================================

If you get this message when you call cmake, it is probably because you gave a
*relative* path for the boost sources ``BOOST_SOURCE_DIR`` (like
``../../../boost_1_47``). You must use an absolute path for
``BOOST_SOURCE_DIR``, like ``/path/to/boost_1_47``.
