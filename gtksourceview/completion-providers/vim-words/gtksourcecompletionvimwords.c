/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8; coding: utf-8 -*-
 * gtksourcecompletionproviderwords.h
 * This file is part of GtkSourceView
 *
 * Copyright (C) 2017 - Umang Jain <mailumangjain@gmail.com>
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


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "gtksourceview/gtksource.h"
#include "gtksourcecompletionvimwordsproposal.h"
#include "gtksourceview/gtksourceview-enumtypes.h"
#include "gtksourceview/gtksourceview-i18n.h"

#include <string.h>

#include "gtksourcecompletionvimwords.h"

enum
{
    PROP_0,
    PROP_NAME,
    PROP_ICON,
    PROP_INTERACTIVE_DELAY,
    PROP_PRIORITY,
    PROP_ACTIVATION,
    N_PROPERTIES
};
static GParamSpec *properties[N_PROPERTIES];

static void gtk_source_completion_vim_words_iface_init (GtkSourceCompletionProviderIface *iface);

struct _GtkSourceCompletionVimWordsPrivate {
	
	GtkSourceSearchContext *search_context;
	GtkSourceSearchSettings *search_settings;
	GtkSourceCompletionContext *context;
  GList *list;

	gchar *word;
	guint cancel_id;

	gchar *name;
    GdkPixbuf *icon;
    gint interactive_delay;
    gint priority;
    GtkSourceCompletionActivation activation;
};

G_DEFINE_TYPE_WITH_CODE (GtkSourceCompletionVimWords,
             gtk_source_completion_vim_words,
             G_TYPE_OBJECT,
             G_ADD_PRIVATE (GtkSourceCompletionVimWords)
             G_IMPLEMENT_INTERFACE (GTK_SOURCE_TYPE_COMPLETION_PROVIDER,
                        gtk_source_completion_vim_words_iface_init))


static gchar *
word_prefix_at_iter (GtkSourceCompletionContext *context)
{
	GtkTextIter end;
	GtkTextIter begin;
	gunichar ch = 0;

	g_return_val_if_fail (GTK_SOURCE_IS_COMPLETION_CONTEXT (context), NULL);

	if (!gtk_source_completion_context_get_iter (context, &end))
	  return NULL;

	begin = end;

	do
	  {
	    if (!gtk_text_iter_backward_char (&begin))
	      break;
	    ch = gtk_text_iter_get_char (&begin);
	  }
	while (g_unichar_isalnum (ch) || (ch == '_'));

	if (ch && !g_unichar_isalnum (ch) && (ch != '_'))
	  gtk_text_iter_forward_char (&begin);

	return gtk_text_iter_get_slice (&begin, &end);
}

static void
forward_search_finished (GtkSourceSearchContext *search_context,
             GAsyncResult           *result,
             GtkSourceCompletionVimWords             *self)
{
  GtkTextIter match_start;
  GtkTextIter match_end;
  gboolean has_wrapped_around;
	GtkSourceCompletionVimWordsProposal *proposal;
	GError *error = NULL;

  if (gtk_source_search_context_forward_finish (search_context,
                              result,
                              &match_start,
                              &match_end,
                              &has_wrapped_around,
                              &error))
	{
		g_print ("TEXT: %s\n", gtk_text_iter_get_text (&match_start, &match_end));
		proposal = gtk_source_completion_vim_words_proposal_new (gtk_text_iter_get_text (&match_start, &match_end));
		self->priv->list = g_list_prepend (self->priv->list, proposal);
        gtk_source_search_context_forward_async (self->priv->search_context,
                         &match_end,
                         NULL,
                         (GAsyncReadyCallback)forward_search_finished,
                         self);
	return;
    }

	self->priv->list = g_list_reverse (self->priv->list);
	    gtk_source_completion_context_add_proposals (self->priv->context,
                                                 GTK_SOURCE_COMPLETION_PROVIDER (self),
                                                 self->priv->list,
                                                 TRUE);

    g_list_free (self->priv->list);	

}

static gchar *
gtk_source_completion_vim_words_get_name (GtkSourceCompletionProvider *self)
{
    return g_strdup (GTK_SOURCE_COMPLETION_VIM_WORDS (self)->priv->name);
}

static GdkPixbuf *
gtk_source_completion_vim_words_get_icon (GtkSourceCompletionProvider *self)
{
    return GTK_SOURCE_COMPLETION_VIM_WORDS (self)->priv->icon;
}

static void
population_finished (GtkSourceCompletionVimWords *self)
{

    g_free (self->priv->word);
    self->priv->word = NULL;

    if (self->priv->context != NULL)
    {
        if (self->priv->cancel_id)
        {
            g_signal_handler_disconnect (self->priv->context,
                                         self->priv->cancel_id);
            self->priv->cancel_id = 0;
        }

        g_clear_object (&self->priv->context);
    }
}

static void
gtk_source_completion_vim_words_populate (GtkSourceCompletionProvider *provider,
																					GtkSourceCompletionContext  *context)
{
	GtkSourceCompletionVimWords *self = GTK_SOURCE_COMPLETION_VIM_WORDS (provider);
	GtkTextIter iter;
	gchar *unescaped_text;

	// NOTE: Return if the completion is not USER_REQUESTED

	if (!gtk_source_completion_context_get_iter (context, &iter))
	{
		gtk_source_completion_context_add_proposals (context, provider, NULL, TRUE);
		return;
	}

	g_free (self->priv->word);
	self->priv->word = NULL;
  
	self->priv->search_settings = gtk_source_search_settings_new ();

  self->priv->search_context = gtk_source_search_context_new (GTK_SOURCE_BUFFER (gtk_text_iter_get_buffer (&iter)),
                                      												self->priv->search_settings);
	self->priv->context = g_object_ref (context);
	self->priv->list = NULL;

	gtk_source_search_settings_set_regex_enabled (self->priv->search_settings, TRUE);
	gtk_source_search_settings_set_at_word_boundaries (self->priv->search_settings, TRUE);

	self->priv->word = word_prefix_at_iter (context);
	unescaped_text = gtk_source_utils_unescape_search_text (self->priv->word);

	gtk_source_search_settings_set_search_text (self->priv->search_settings, "ide[a-zA-Z0-9_]*"); // get regex here
  g_free (unescaped_text);

	self->priv->cancel_id = g_signal_connect_swapped (context,
																										 "cancelled",
                              											 G_CALLBACK (population_finished),
                              											 self);

	gtk_source_search_context_forward_async (self->priv->search_context,
  									                       &iter,
                    									     NULL,
                         									 (GAsyncReadyCallback)forward_search_finished,
                        									 self);
}

static gboolean
gtk_source_completion_vim_words_get_start_iter (GtkSourceCompletionProvider *provider,
																								GtkSourceCompletionContext  *context,
                                            		GtkSourceCompletionProposal *proposal,
                                            		GtkTextIter                 *iter)
{
	gchar *word;
	glong nb_chars;

	if (!gtk_source_completion_context_get_iter (context, iter))
		return FALSE;

	word = word_prefix_at_iter (context);
  g_return_val_if_fail (word != NULL, FALSE);

  nb_chars = g_utf8_strlen (word, -1);
  gtk_text_iter_backward_chars (iter, nb_chars);

  g_free (word);
  return TRUE;
}

static gint
gtk_source_completion_vim_words_get_interactive_delay (GtkSourceCompletionProvider *provider)
{
  return GTK_SOURCE_COMPLETION_VIM_WORDS (provider)->priv->interactive_delay;
}

static gint
gtk_source_completion_vim_words_get_priority (GtkSourceCompletionProvider *provider)
{
  return GTK_SOURCE_COMPLETION_VIM_WORDS (provider)->priv->priority;
}

static GtkSourceCompletionActivation
gtk_source_completion_vim_words_get_activation (GtkSourceCompletionProvider *provider)
{
  return GTK_SOURCE_COMPLETION_VIM_WORDS (provider)->priv->activation;
}


static void gtk_source_completion_vim_words_iface_init (GtkSourceCompletionProviderIface *iface)
{
	iface->get_name = gtk_source_completion_vim_words_get_name;
	iface->get_icon = gtk_source_completion_vim_words_get_icon;
	iface->populate = gtk_source_completion_vim_words_populate;
	iface->get_start_iter = gtk_source_completion_vim_words_get_start_iter;
	iface->get_interactive_delay = gtk_source_completion_vim_words_get_interactive_delay;
	iface->get_priority = gtk_source_completion_vim_words_get_priority;
	iface->get_activation = gtk_source_completion_vim_words_get_activation;
}

static void
gtk_source_completion_vim_words_set_property (GObject      *object,
                                          		guint         prop_id,
                                          		const GValue *value,
                                          		GParamSpec   *pspec)
{
    GtkSourceCompletionVimWords *self = GTK_SOURCE_COMPLETION_VIM_WORDS (object);

    switch (prop_id)
    {
        case PROP_NAME:
            g_free (self->priv->name);
            self->priv->name = g_value_dup_string (value);

            if (self->priv->name == NULL)
            {
                self->priv->name = g_strdup (_("Document Vim Words"));
            }
            break;

        case PROP_ICON:
            g_clear_object (&self->priv->icon);
            self->priv->icon = g_value_dup_object (value);
            break;

        case PROP_INTERACTIVE_DELAY:
            self->priv->interactive_delay = g_value_get_int (value);
            break;

        case PROP_PRIORITY:
            self->priv->priority = g_value_get_int (value);
            break;

        case PROP_ACTIVATION:
            self->priv->activation = g_value_get_flags (value);
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
    }
}

static void
gtk_source_completion_vim_words_dispose (GObject *object)
{
  GtkSourceCompletionVimWords *self = GTK_SOURCE_COMPLETION_VIM_WORDS (object);

  population_finished (self);

  g_free (self->priv->name);
  self->priv->name = NULL;

  g_clear_object (&self->priv->icon);
	g_clear_object (&self->priv->search_context);

  G_OBJECT_CLASS (gtk_source_completion_vim_words_parent_class)->dispose (object);
}

static void
gtk_source_completion_vim_words_get_property (GObject    *object,
                                          		guint       prop_id,
                                          		GValue     *value,
                                          		GParamSpec *pspec)
{
    GtkSourceCompletionVimWords *self = GTK_SOURCE_COMPLETION_VIM_WORDS (object);

    switch (prop_id)
    {
        case PROP_NAME:
            g_value_set_string (value, self->priv->name);
            break;

        case PROP_ICON:
            g_value_set_object (value, self->priv->icon);
            break;

        case PROP_INTERACTIVE_DELAY:
            g_value_set_int (value, self->priv->interactive_delay);
            break;

        case PROP_PRIORITY:
            g_value_set_int (value, self->priv->priority);
            break;

        case PROP_ACTIVATION:
            g_value_set_flags (value, self->priv->activation);
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
    }
}


static void
gtk_source_completion_vim_words_class_init (GtkSourceCompletionVimWordsClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);

    object_class->get_property = gtk_source_completion_vim_words_get_property;
    object_class->set_property = gtk_source_completion_vim_words_set_property;
    object_class->dispose = gtk_source_completion_vim_words_dispose;

    properties[PROP_NAME] =
        g_param_spec_string ("name",
                     "Name",
                     "The provider name",
                     NULL,
                     G_PARAM_READWRITE | G_PARAM_CONSTRUCT | G_PARAM_STATIC_STRINGS);

    properties[PROP_ICON] =
        g_param_spec_object ("icon",
                     "Icon",
                     "The provider icon",
                     GDK_TYPE_PIXBUF,
                     G_PARAM_READWRITE | G_PARAM_CONSTRUCT | G_PARAM_STATIC_STRINGS);

    properties[PROP_INTERACTIVE_DELAY] =
        g_param_spec_int ("interactive-delay",
                  "Interactive Delay",
                  "The delay before initiating interactive completion",
                  -1,
                  G_MAXINT,
                  50,
                  G_PARAM_READWRITE | G_PARAM_CONSTRUCT | G_PARAM_STATIC_STRINGS);

    properties[PROP_PRIORITY] =
        g_param_spec_int ("priority",
                  "Priority",
                  "Provider priority",
                  G_MININT,
                  G_MAXINT,
                  0,
                  G_PARAM_READWRITE | G_PARAM_CONSTRUCT | G_PARAM_STATIC_STRINGS);

    /**
     * GtkSourceCompletionWords:activation:
     *
     * The type of activation.
     *
     * Since: 3.10
     */
    properties[PROP_ACTIVATION] =
        g_param_spec_flags ("activation",
                    "Activation",
                    "The type of activation",
                    GTK_SOURCE_TYPE_COMPLETION_ACTIVATION,
                    GTK_SOURCE_COMPLETION_ACTIVATION_INTERACTIVE |
                    GTK_SOURCE_COMPLETION_ACTIVATION_USER_REQUESTED,
                    G_PARAM_READWRITE | G_PARAM_CONSTRUCT | G_PARAM_STATIC_STRINGS);

    g_object_class_install_properties (object_class, N_PROPERTIES, properties);
}

static void
gtk_source_completion_vim_words_init (GtkSourceCompletionVimWords *self)
{
	self->priv = gtk_source_completion_vim_words_get_instance_private (self);
}

GtkSourceCompletionVimWords *
gtk_source_completion_vim_words_new (const gchar *name,
                                 		 GdkPixbuf   *icon)
{
    return g_object_new (GTK_SOURCE_TYPE_COMPLETION_VIM_WORDS,
                         "name", name,
                         "icon", icon,
                         NULL);
}
