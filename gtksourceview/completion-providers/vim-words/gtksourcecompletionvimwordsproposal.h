/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8; coding: utf-8 -*-
 * gtksourcecompletionvim_wordsproposal.h
 * This file is part of GtkSourceView
 *
 * Copyright (C) 2009 - Jesse van den Kieboom
 *
 * gtksourceview is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * gtksourceview is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GTK_SOURCE_COMPLETION_VIM_WORDS_PROPOSAL_H
#define GTK_SOURCE_COMPLETION_VIM_WORDS_PROPOSAL_H

#include <glib-object.h>
#include <gtksourceview/gtksourcecompletionproposal.h>

#include "gtksourceview/gtksourcetypes-private.h"

G_BEGIN_DECLS

#define GTK_SOURCE_TYPE_COMPLETION_VIM_WORDS_PROPOSAL			(gtk_source_completion_vim_words_proposal_get_type ())
#define GTK_SOURCE_COMPLETION_VIM_WORDS_PROPOSAL(obj)			(G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_SOURCE_TYPE_COMPLETION_VIM_WORDS_PROPOSAL, GtkSourceCompletionVimWordsProposal))
#define GTK_SOURCE_COMPLETION_VIM_WORDS_PROPOSAL_CONST(obj)			(G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_SOURCE_TYPE_COMPLETION_VIM_WORDS_PROPOSAL, GtkSourceCompletionVimWordsProposal const))
#define GTK_SOURCE_COMPLETION_VIM_WORDS_PROPOSAL_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST ((klass), GTK_SOURCE_TYPE_COMPLETION_VIM_WORDS_PROPOSAL, GtkSourceCompletionVimWordsProposalClass))
#define GTK_SOURCE_IS_COMPLETION_VIM_WORDS_PROPOSAL(obj)			(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_SOURCE_TYPE_COMPLETION_VIM_WORDS_PROPOSAL))
#define GTK_SOURCE_IS_COMPLETION_VIM_WORDS_PROPOSAL_CLASS(klass)		(G_TYPE_CHECK_CLASS_TYPE ((klass), GTK_SOURCE_TYPE_COMPLETION_VIM_WORDS_PROPOSAL))
#define GTK_SOURCE_COMPLETION_VIM_WORDS_PROPOSAL_GET_CLASS(obj)		(G_TYPE_INSTANCE_GET_CLASS ((obj), GTK_SOURCE_TYPE_COMPLETION_VIM_WORDS_PROPOSAL, GtkSourceCompletionVimWordsProposalClass))

typedef struct _GtkSourceCompletionVimWordsProposal		GtkSourceCompletionVimWordsProposal;
typedef struct _GtkSourceCompletionVimWordsProposalClass		GtkSourceCompletionVimWordsProposalClass;
typedef struct _GtkSourceCompletionVimWordsProposalPrivate		GtkSourceCompletionVimWordsProposalPrivate;

struct _GtkSourceCompletionVimWordsProposal {
	GObject parent;

	GtkSourceCompletionVimWordsProposalPrivate *priv;
};

struct _GtkSourceCompletionVimWordsProposalClass {
	GObjectClass parent_class;
};

GTK_SOURCE_INTERNAL
GType		 gtk_source_completion_vim_words_proposal_get_type	(void) G_GNUC_CONST;

GTK_SOURCE_INTERNAL
GtkSourceCompletionVimWordsProposal *
		 gtk_source_completion_vim_words_proposal_new 	(const gchar                      *word);

GTK_SOURCE_INTERNAL
const gchar 	*gtk_source_completion_vim_words_proposal_get_word 	(GtkSourceCompletionVimWordsProposal *proposal);

GTK_SOURCE_INTERNAL
void		 gtk_source_completion_vim_words_proposal_use 	(GtkSourceCompletionVimWordsProposal *proposal);

GTK_SOURCE_INTERNAL
void		 gtk_source_completion_vim_words_proposal_unuse 	(GtkSourceCompletionVimWordsProposal *proposal);

G_END_DECLS

#endif /* GTK_SOURCE_COMPLETION_VIM_WORDS_PROPOSAL_H */
