#!/usr/bin/env python3
# This file is a part of marzer/soagen and is subject to the the terms of the MIT license.
# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
# SPDX-License-Identifier: MIT

import re
from io import StringIO
from pathlib import Path

from . import log, utils

#-----------------------------------------------------------------------------------------------------------------------
# code file writer object
#-----------------------------------------------------------------------------------------------------------------------



class Writer(object):

	def __init__(self, path, meta=None, indent='\t', clang_format=False, doxygen=True):
		assert isinstance(path, Path)
		assert isinstance(indent, str)
		self.__path = path.resolve()
		self.__buffer = StringIO()
		self.__indent = 0
		self.__indent_str = str(indent)
		self.__indent_width = len(utils.untabify(self.__indent_str))
		self.__meta = meta
		self.__clang_format = bool(clang_format)
		self.__doxygen = bool(doxygen)
		self.__cpp_access_level = None
		self.emit_access_levels = True

	def write(self, text=None, end='\n', reindent=True):
		if text is None:
			text = ''
		elif not isinstance(text, str):
			text = str(text)
		if end is None:
			end = ''
		if end or text:
			if text and reindent:
				text = utils.reindent(text, indent=self.__indent_str * self.__indent)
			text += end
			if self.__meta is not None:
				text = self.__meta(text)
			self.__buffer.write(text)
		return self

	def __call__(self, text=None, **kwargs):
		return self.write(text=text, **kwargs)

	@property
	def indent(self) -> int:
		return self.__indent

	@indent.setter
	def indent(self, value: int):
		self.__indent = int(value)
		assert self.__indent >= 0

	@property
	def indent_width(self) -> int:
		return self.__indent * self.__indent_width

	@property
	def clang_format(self) -> bool:
		return self.__clang_format

	@clang_format.setter
	def clang_format(self, value: bool):
		self.__clang_format = bool(value)

	@property
	def doxygen(self) -> bool:
		return self.__doxygen

	@doxygen.setter
	def doxygen(self, value: bool):
		self.__doxygen = bool(value)

	@property
	def cpp_access_level(self) -> str:
		return self.__cpp_access_level

	@cpp_access_level.setter
	def cpp_access_level(self, value: str):
		assert value is None or value in (r'public', r'protected', r'private')
		if self.__cpp_access_level == value:
			return
		if self.__cpp_access_level == r'private' and self.doxygen and self.emit_access_levels:
			self('/// \endcond')
		self.__cpp_access_level = value
		if self.__cpp_access_level is not None and self.emit_access_levels:
			self()
			self(rf'{self.__cpp_access_level}:')
			if self.__cpp_access_level == r'private' and self.doxygen:
				self('/// \cond')

	def flush(self):
		s = self.__buffer.getvalue().lstrip()
		# minor cleanup
		s = re.sub(r'\n(?:[ \t]*\n[ \t]*)+\n', '\n\n', s)  # double blank lines
		s = re.sub(r'([^@][({,])\n\n', r'\1\n', s)  # blank lines following opening brackets or a comma
		s = re.sub(r'\n\n([ \t]*[})])', r'\n\1', s)  # blank lines preceeding closing brackets
		# clang-format
		if self.__clang_format:
			log.i(f"Running clang-format for {self.__path}")
			try:
				s = utils.clang_format(s, cwd=self.__path.parent)
			except Exception as ex:
				log.w(f"Clang-format failed: {ex}")
		# store formatting changes
		self.__buffer.seek(0, 0)
		self.__buffer.truncate()
		self.__buffer.write(s)
		# write to disk
		log.i(f"Writing {self.__path}")
		with open(self.__path, 'w', encoding='utf-8', newline='\n') as f:
			f.write(s)
		return self

	def __enter__(self):
		return self

	def __exit__(self, exc_type, exc_value, exc_traceback):
		self.flush()



#-----------------------------------------------------------------------------------------------------------------------
# logical blocks within a file
#-----------------------------------------------------------------------------------------------------------------------



class WriterBlockBase(object):

	def __init__(self, writer: Writer):
		assert isinstance(writer, Writer)
		self.__writer = writer

	@property
	def writer(self) -> Writer:
		return self.__writer



class Indent(WriterBlockBase):

	def __init__(self, writer: Writer, pre='', post=''):
		super().__init__(writer)
		self.__pre = pre
		self.__post = post

	def __enter__(self):
		if self.__pre:
			self.writer(self.__pre)
		self.writer.indent += 1
		return self

	def __exit__(self, exc_type, exc_value, exc_traceback):
		self.writer.indent -= 1
		if self.__post:
			self.writer(self.__post)



class Namespace(WriterBlockBase):

	def __init__(self, writer: Writer, ns):
		super().__init__(writer)
		assert ns is None or isinstance(ns, str)
		self.__ok = False
		if ns is None:
			self.__ok = True
			self.__namespace = ''
		elif ns:
			self.__ok = True
			self.__namespace = ns

	def __enter__(self):
		if self.__ok:
			self.writer(f'namespace {self.__namespace}')
			self.writer('{')
			self.writer.indent += 1
		return self

	def __exit__(self, exc_type, exc_value, exc_traceback):
		if self.__ok:
			self.writer.indent -= 1
			self.writer('}')



class Block(WriterBlockBase):

	def __init__(self, writer: Writer, pre='', post=''):
		super().__init__(writer)
		self.__pre = pre
		self.__post = post

	def __enter__(self):
		self.writer(rf'{self.__pre}{{')
		self.writer.indent += 1
		return self

	def __exit__(self, exc_type, exc_value, exc_traceback):
		self.writer.indent -= 1
		self.writer(rf'}}{self.__post}')



class ClassDefinition(WriterBlockBase):

	def __init__(self, writer: Writer, class_):
		super().__init__(writer)
		self.__class = class_
		self.__prev_cpp_access_level = None

	def __enter__(self):
		self.__prev_cpp_access_level = self.writer.cpp_access_level
		self.writer.cpp_access_level = None
		self.writer(self.__class)
		self.writer('{')
		self.writer.indent += 1
		return self

	def __exit__(self, exc_type, exc_value, exc_traceback):
		self.writer.indent -= 1
		self.writer('};')
		self.writer.emit_access_levels = False
		self.writer.cpp_access_level = self.__prev_cpp_access_level
		self.writer.emit_access_levels = True



class Private(WriterBlockBase):

	def __enter__(self):
		self.writer.cpp_access_level = r'private'
		self.writer.indent += 1
		return self

	def __exit__(self, exc_type, exc_value, exc_traceback):
		self.writer.indent -= 1
		self.writer()



class Protected(WriterBlockBase):

	def __enter__(self):
		self.writer.cpp_access_level = r'protected'
		self.writer.indent += 1
		return self

	def __exit__(self, exc_type, exc_value, exc_traceback):
		self.writer.indent -= 1
		self.writer()



class Public(WriterBlockBase):

	def __enter__(self):
		self.writer.cpp_access_level = r'public'
		self.writer.indent += 1
		return self

	def __exit__(self, exc_type, exc_value, exc_traceback):
		self.writer.indent -= 1
		self.writer()



class DoxygenGroup(WriterBlockBase):

	def __init__(self, writer: Writer, key, name):
		super().__init__(writer)
		self.__key = key
		self.__name = name

	def __enter__(self):
		if self.writer.doxygen:
			self.writer(rf'''
			/// \addtogroup {self.__key} {self.__name}
			/// @{{
			''')
		return self

	def __exit__(self, exc_type, exc_value, exc_traceback):
		if self.writer.doxygen:
			self.writer(r'''
			/// @}
			''')



class DoxygenMemberGroup(WriterBlockBase):

	def __init__(self, writer: Writer, name):
		super().__init__(writer)
		self.__name = name

	def __enter__(self):
		if self.writer.doxygen:
			self.writer(rf'''
			/// \name {self.__name}
			/// @{{
			''')
		return self

	def __exit__(self, exc_type, exc_value, exc_traceback):
		if self.writer.doxygen:
			self.writer(r'''
			/// @}
			''')



class HiddenFromDoxygen(WriterBlockBase):

	def __enter__(self):
		if self.writer.doxygen:
			self.writer(rf'''
			/// \cond
			''')
		return self

	def __exit__(self, exc_type, exc_value, exc_traceback):
		if self.writer.doxygen:
			self.writer(r'''
			/// \endcond
			''')



class PreprocessorRegion(WriterBlockBase):

	def __enter__(self):
		self.writer(r'''#if 1
		''')
		return self

	def __exit__(self, exc_type, exc_value, exc_traceback):
		self.writer(r'''
		#endif''')



__all__ = [
	r'WriterBlockBase', r'Writer', r'Indent', r'Namespace', r'Block', r'ClassDefinition', r'Private', r'Protected',
	r'Public', r'DoxygenGroup', r'DoxygenMemberGroup', r'HiddenFromDoxygen', r'PreprocessorRegion'
]
