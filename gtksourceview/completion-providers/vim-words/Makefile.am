@CODE_COVERAGE_RULES@

AM_CPPFLAGS = 				\
	-DDATADIR=\""$(datadir)"\" 	\
	-DG_LOG_DOMAIN=\"GtkSourceView\"\
	-DGTK_SOURCE_COMPILATION	\
	-I$(top_srcdir) -I$(srcdir) 	\
	$(DISABLE_DEPRECATED_CFLAGS)	\
	$(WARN_CFLAGS) 			\
	$(CODE_COVERAGE_CPPFLAGS)	\
	$(DEP_CFLAGS)

noinst_LTLIBRARIES = libgtksourcecompletionvimwords.la

NOINST_H_FILES =	\
	gtksourcecompletionvimwordsproposal.h

libgtksourcecompletionvimwords_headers = 		\
	gtksourcecompletionvimwords.h

libgtksourcecompletionvimwords_la_SOURCES =	\
	gtksourcecompletionvimwords.c		\
	gtksourcecompletionvimwordsproposal.c	\
	$(libgtksourcecompletionvimwords_headers)	\
	$(NOINST_H_FILES)

libgtksourcecompletionvimwords_la_CFLAGS =		\
	$(HIDDEN_VISIBILITY_CFLAGS)		\
	$(CODE_COVERAGE_CFLAGS)

libgtksourcecompletionvimwords_la_LDFLAGS =	\
	$(WARN_LDFLAGS)

libgtksourcecompletionvimwords_la_LIBADD =		\
	$(CODE_COVERAGE_LIBS)

libgtksourcecompletionvimwords_includedir = \
	$(includedir)/gtksourceview-@GSV_API_VERSION@/gtksourceview/completion-providers/vim-words

libgtksourcecompletionvimwords_include_HEADERS =		\
	$(libgtksourcecompletionvimwords_headers)

-include $(top_srcdir)/git.mk
