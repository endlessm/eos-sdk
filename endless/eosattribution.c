/* Copyright 2015 Endless Mobile, Inc. */

#include "config.h"
#include <glib/gi18n-lib.h>
#include <gtk/gtk.h>
#include <json-glib/json-glib.h>

#include "eosattribution-private.h"
#include "eoscellrendererpixbuflink-private.h"
#include "eoscellrenderertextlink-private.h"

typedef struct
{
  GFile *file;
  GtkWidget *view;
  GtkListStore *model;
} EosAttributionPrivate;

/* Forward declarations */
static void eos_attribution_init_initable (GInitableIface *, GInterfaceInfo *);

G_DEFINE_TYPE_WITH_CODE (EosAttribution, eos_attribution,
                         GTK_TYPE_SCROLLED_WINDOW,
                         G_ADD_PRIVATE (EosAttribution)
                         G_IMPLEMENT_INTERFACE (G_TYPE_INITABLE,
                                                eos_attribution_init_initable))

#define ROW_HEIGHT 50  /* Height of the pixbufs in each row */

enum {
  SHOW_URI,
  LAST_SIGNAL
};

static guint attribution_signals[LAST_SIGNAL] = { 0 };

enum
{
  PROP_0,
  PROP_FILE,
  NPROPS
};

static GParamSpec *eos_attribution_props[NPROPS] = { NULL, };

/* These are the recognized string values for the "license" field. Any other
license must be clarified in the comments, or linked to with the "license_uri"
field. Make sure to add new values to the table "image-attribution-licenses" in
the documentation of EosApplication and to the two arrays below this one. */
static gchar * const recognized_licenses[] = {
  "Public domain",
  "CC0 1.0",
  "CC BY 2.0",
  "CC BY 3.0",
  "CC BY-SA 2.0",
  "CC BY-SA 3.0",
  "CC BY-ND 2.0",
  "CC BY-ND 3.0",
  NULL
};

static gchar * const license_display_names[] = {
  /* TRANSLATORS: These names should be translated as the official names of the
  licenses in your language. */
  N_("Public domain"),
  N_("CC0 1.0 (Public domain)"),
  N_("Creative Commons Attribution 2.0"),
  N_("Creative Commons Attribution 3.0"),
  N_("Creative Commons Attribution-ShareAlike 2.0"),
  N_("Creative Commons Attribution-ShareAlike 3.0"),
  N_("Creative Commons Attribution-NoDerivs 2.0"),
  N_("Creative Commons Attribution-NoDerivs 3.0"),
  NULL
};

static gchar * const license_uris[] = {
  NULL,
  /* TRANSLATORS: These URIs should be translated as a link to the license page
  in your language. For example, for Spanish,
  "http://creativecommons.org/licenses/by/3.0/" should become
  "http://creativecommons.org/licenses/by/3.0/deed.es". However, if no such page
  exists, then leave these untranslated. */
  N_("http://creativecommons.org/publicdomain/zero/1.0/"),
  N_("http://creativecommons.org/licenses/by/2.0/"),
  N_("http://creativecommons.org/licenses/by/3.0/"),
  N_("http://creativecommons.org/licenses/by-sa/2.0/"),
  N_("http://creativecommons.org/licenses/by-sa/3.0/"),
  N_("http://creativecommons.org/licenses/by-nd/2.0/"),
  N_("http://creativecommons.org/licenses/by-nd/3.0/"),
  NULL
};

enum
{
  LICENSE_PUBLIC_DOMAIN,
  LICENSE_CC0,
  LICENSE_CC_BY_2_0,
  LICENSE_CC_BY_3_0,
  LICENSE_CC_BY_SA_2_0,
  LICENSE_CC_BY_SA_3_0,
  LICENSE_GFDL_1_2,
  LICENSE_GFDL_1_3,
  NUM_RECOGNIZED_LICENSES
};

enum
{
  COLUMN_PIXBUF,
  COLUMN_ORIGINAL_URI,
  COLUMN_LICENSE,
  COLUMN_LICENSE_URI,
  COLUMN_CREDIT,
  COLUMN_CREDIT_CONTACT,
  COLUMN_COPYRIGHT_HOLDER,
  COLUMN_COPYRIGHT_YEAR,
  COLUMN_PERMISSION,
  COLUMN_COMMENT,
  NUM_MODEL_COLUMNS
};

static void
eos_attribution_get_property (GObject    *object,
                              guint       property_id,
                              GValue     *value,
                              GParamSpec *pspec)
{
  EosAttribution *self = EOS_ATTRIBUTION (object);

  switch (property_id)
    {
    case PROP_FILE:
      g_value_set_object (value, eos_attribution_get_file (self));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    }
}

static void
eos_attribution_set_property (GObject      *object,
                              guint         property_id,
                              const GValue *value,
                              GParamSpec   *pspec)
{
  EosAttribution *self = EOS_ATTRIBUTION (object);
  EosAttributionPrivate *priv = eos_attribution_get_instance_private (self);

  switch (property_id)
    {
    case PROP_FILE:
      priv->file = g_value_dup_object (value);  /* construct only */
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    }
}

static void
eos_attribution_finalize (GObject *object)
{
  EosAttribution *self = EOS_ATTRIBUTION (object);
  EosAttributionPrivate *priv = eos_attribution_get_instance_private (self);

  g_object_unref (priv->model);

  G_OBJECT_CLASS (eos_attribution_parent_class)->finalize (object);
}

static void
eos_attribution_class_init (EosAttributionClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->get_property = eos_attribution_get_property;
  object_class->set_property = eos_attribution_set_property;
  object_class->finalize = eos_attribution_finalize;

  attribution_signals[SHOW_URI] =
    g_signal_new ("show-uri", EOS_TYPE_ATTRIBUTION,
                  G_SIGNAL_RUN_FIRST,
                  0,
                  NULL, NULL,
                  g_cclosure_marshal_VOID__STRING,
                  G_TYPE_NONE,
                  1, G_TYPE_STRING);

  eos_attribution_props[PROP_FILE] =
    g_param_spec_object ("file", "File",
                         "JSON file with attribution information for images",
                         G_TYPE_FILE,
                         G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS);

  g_object_class_install_properties (object_class, NPROPS,
                                     eos_attribution_props);
}

static void
on_pixbuf_cell_clicked (GtkCellRenderer *renderer,
                        const gchar     *path,
                        EosAttribution  *self)
{
  EosAttributionPrivate *priv = eos_attribution_get_instance_private (self);
  GtkTreeIter iter;
  gchar *original_uri;
  GtkTreeModel *model = GTK_TREE_MODEL (priv->model);

  if (!gtk_tree_model_get_iter_from_string (model, &iter, path))
    {
      g_warning ("Apparently someone clicked on a nonexistent cell renderer.");
      return;
    }
  gtk_tree_model_get (model, &iter,
                      COLUMN_ORIGINAL_URI, &original_uri,
                      -1);
  if (original_uri != NULL)
    g_signal_emit (self, attribution_signals[SHOW_URI], 0, original_uri);
  g_free (original_uri);
}

static void
on_license_cell_clicked (GtkCellRenderer *renderer,
                         const gchar     *path,
                         EosAttribution  *self)
{
  EosAttributionPrivate *priv = eos_attribution_get_instance_private (self);
  GtkTreeIter iter;
  gchar *license_uri;
  GtkTreeModel *model = GTK_TREE_MODEL (priv->model);

  if (!gtk_tree_model_get_iter_from_string (model, &iter, path))
    {
      g_warning ("Apparently someone clicked on a nonexistent cell renderer.");
      return;
    }
  gtk_tree_model_get (model, &iter,
                      COLUMN_LICENSE_URI, &license_uri,
                      -1);
  if (license_uri != NULL)
    g_signal_emit (self, attribution_signals[SHOW_URI], 0, license_uri);
  g_free (license_uri);
}

static void
on_contact_cell_clicked (GtkCellRenderer *renderer,
                         const gchar     *path,
                         EosAttribution  *self)
{
  EosAttributionPrivate *priv = eos_attribution_get_instance_private (self);
  GtkTreeIter iter;
  gchar *contact;
  GtkTreeModel *model = GTK_TREE_MODEL (priv->model);

  if (!gtk_tree_model_get_iter_from_string (model, &iter, path))
    {
      g_warning ("Apparently someone clicked on a nonexistent cell renderer.");
      return;
    }
  gtk_tree_model_get (model, &iter,
                      COLUMN_CREDIT_CONTACT, &contact,
                      -1);
  if (contact != NULL)
    g_signal_emit (self, attribution_signals[SHOW_URI], 0, contact);
  g_free (contact);
}

static void
render_license_link (GtkTreeViewColumn *column,
                     GtkCellRenderer   *renderer,
                     GtkTreeModel      *model,
                     GtkTreeIter       *iter)
{
  gchar *license_uri;
  gint license_index;
  gtk_tree_model_get (model, iter,
                      COLUMN_LICENSE, &license_index,
                      COLUMN_LICENSE_URI, &license_uri,
                      -1);
  if (license_index != -1)
    {
      /* TRANSLATORS: %s will be replaced with the name of an image license,
      such as "Public domain" or "Creative Commons Attribution". These names are
      translated elsewhere in this file. Make sure %s is still in the translated
      string. */
      gchar *license_string = g_strdup_printf (_("%s."),
                                               gettext (license_display_names[license_index]));
      gboolean behave_like_link = (license_uri != NULL);
      g_object_set (renderer,
                    "markup", license_string,
                    "visible", TRUE,
                    "foreground-set", behave_like_link,
                    NULL);
      g_free (license_string);
    }
  else if (license_uri != NULL)
    {
      g_object_set (renderer,
                    "markup", _("Click for image license."),
                    "visible", TRUE,
                    NULL);
    }
  else
    {
      g_object_set (renderer, "visible", FALSE, NULL);
    }

  g_free (license_uri);
}

static void
render_contact_link (GtkTreeViewColumn *column,
                     GtkCellRenderer   *renderer,
                     GtkTreeModel      *model,
                     GtkTreeIter       *iter)
{
  gchar *credit, *credit_contact;
  gtk_tree_model_get (model, iter,
                      COLUMN_CREDIT, &credit,
                      COLUMN_CREDIT_CONTACT, &credit_contact,
                      -1);
  if (credit != NULL)
    {
      /* TRANSLATORS: %s will be replaced with the name or account name of the
      person that the image should be credited to. Make sure %s is still in the
      translated string. */
      gchar *credit_string = g_strdup_printf (_("Image credit: %s."), credit);
      g_object_set (renderer,
                    "markup", credit_string,
                    "visible", TRUE,
                    "foreground-set", FALSE,
                    NULL);
      g_free (credit_string);
    }
  else
    {
      g_object_set (renderer, "visible", FALSE, NULL);
    }

  g_free (credit);
  g_free (credit_contact);
}

static void
render_usage_notes (GtkTreeViewColumn *column,
                    GtkCellRenderer   *renderer,
                    GtkTreeModel      *model,
                    GtkTreeIter       *iter)
{
  gchar *copyright_holder, *comment;
  gint copyright_year;
  gboolean permission;
  GString *builder = g_string_new ("");

  gtk_tree_model_get (model, iter,
                      COLUMN_COPYRIGHT_HOLDER, &copyright_holder,
                      COLUMN_COPYRIGHT_YEAR, &copyright_year,
                      COLUMN_PERMISSION, &permission,
                      COLUMN_COMMENT, &comment,
                      -1);
  if (copyright_holder != NULL)
    {
      if (copyright_year != -1)
        {
          /* TRANSLATORS: %d will be replaced with the copyright year, %s with
          the copyright holder. Make sure these tokens are in the translated
          string. */
          g_string_append_printf (builder, _("Copyright %d %s."),
                                  copyright_year, copyright_holder);
        }
      else
        {
          /* TRANSLATORS: %s will be replaced with the name of the copyright
          holder. Make sure %s is still in the translated string. */
          g_string_append_printf (builder, _("Copyright %s."),
                                  copyright_holder);
        }
      if (permission || comment != NULL)
        g_string_append_c (builder, ' ');
    }
  if (permission)
    {
      g_string_append (builder, _("Used with permission."));
      if (comment != NULL)
        g_string_append_c (builder, ' ');
    }
  if (comment != NULL)
    g_string_append (builder, comment);

  g_free (copyright_holder);
  g_free (comment);

  gchar *resulting_text = g_string_free (builder, FALSE);
  g_object_set (renderer, "markup", resulting_text, NULL);
  g_free (resulting_text);
}

static void
eos_attribution_init (EosAttribution *self)
{
  EosAttributionPrivate *priv = eos_attribution_get_instance_private (self);
  priv->model = gtk_list_store_new (NUM_MODEL_COLUMNS,
                                    GDK_TYPE_PIXBUF,
                                    G_TYPE_STRING,
                                    G_TYPE_INT,
                                    G_TYPE_STRING,
                                    G_TYPE_STRING,
                                    G_TYPE_STRING,
                                    G_TYPE_STRING,
                                    G_TYPE_INT,
                                    G_TYPE_BOOLEAN,
                                    G_TYPE_STRING);
  priv->view = gtk_tree_view_new_with_model (GTK_TREE_MODEL (priv->model));
  gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (priv->view), FALSE);

  GtkTreeViewColumn *column;
  GtkCellRenderer *renderer;

  /* Column showing a reduced representation of the image */
  renderer = eos_cell_renderer_pixbuf_link_new ();
  g_signal_connect (renderer, "clicked",
                    G_CALLBACK (on_pixbuf_cell_clicked), self);
  column = gtk_tree_view_column_new_with_attributes ("", renderer,
                                                     "pixbuf", COLUMN_PIXBUF,
                                                     NULL);
  gtk_tree_view_append_column (GTK_TREE_VIEW (priv->view), column);

  GtkCellArea *area = gtk_cell_area_box_new ();
  gtk_orientable_set_orientation (GTK_ORIENTABLE (area),
                                  GTK_ORIENTATION_HORIZONTAL);

  /* Put all the text renderers in the same column */
  column = gtk_tree_view_column_new_with_area (area);
  gtk_tree_view_append_column (GTK_TREE_VIEW (priv->view), column);

  /* Renderer for license link */
  renderer = eos_cell_renderer_text_link_new ();
  g_signal_connect (renderer, "clicked",
                    G_CALLBACK (on_license_cell_clicked), self);
  gtk_cell_area_box_pack_start (GTK_CELL_AREA_BOX (area), renderer,
                                FALSE, FALSE, FALSE);
  gtk_tree_view_column_set_cell_data_func (column, renderer,
                                           (GtkTreeCellDataFunc)render_license_link,
                                           NULL, NULL);

  /* Renderer for original image link */
  renderer = eos_cell_renderer_text_link_new ();
  g_signal_connect (renderer, "clicked",
                    G_CALLBACK (on_contact_cell_clicked), self);
  gtk_cell_area_box_pack_start (GTK_CELL_AREA_BOX (area), renderer,
                                FALSE, FALSE, FALSE);
  gtk_tree_view_column_set_cell_data_func (column, renderer,
                                           (GtkTreeCellDataFunc)render_contact_link,
                                           NULL, NULL);

  /* Renderer for general notes */
  renderer = gtk_cell_renderer_text_new ();
  gtk_cell_area_box_pack_start (GTK_CELL_AREA_BOX (area), renderer,
                                FALSE, FALSE, FALSE);
  gtk_tree_view_column_set_cell_data_func (column, renderer,
                                           (GtkTreeCellDataFunc)render_usage_notes,
                                           NULL, NULL);

  gtk_container_add (GTK_CONTAINER (self), priv->view);
}

/* Utility function, returns the index if an array of strings @strv terminated
by NULL contains the string @entry, -1 if it does not */
static gint strv_index (gchar * const *, const gchar *) G_GNUC_PURE;
static gint
strv_index (gchar * const *strv, const gchar *entry)
{
  if (strv == NULL)
    return -1;

  gchar * const *iter;
  gint index;
  for (iter = strv, index = 0; *iter != NULL; iter++, index++)
    {
      if (strcmp (*iter, entry) == 0)
        return index;
    }
  return -1;
}

static gboolean
eos_attribution_initable_init (GInitable    *initable,
                               GCancellable *cancellable,
                               GError      **error)
{
  EosAttribution *self = EOS_ATTRIBUTION (initable);
  EosAttributionPrivate *priv = eos_attribution_get_instance_private (self);

  GInputStream *stream = G_INPUT_STREAM (g_file_read (priv->file, cancellable,
                                                      error));
  if (stream == NULL)
    return FALSE;

  JsonParser *parser = json_parser_new ();
  gboolean success = json_parser_load_from_stream (parser, stream, cancellable,
                                                   error);
  if (!success)
    {
      g_object_unref (stream);
      goto fail;
    }

  success = g_input_stream_close (stream, cancellable, error);
  g_object_unref (stream);
  if (!success && g_error_matches (*error, G_IO_ERROR, G_IO_ERROR_CANCELLED))
    goto fail;
  /* Ignore errors other than cancellation */

  JsonReader *reader = json_reader_new (json_parser_get_root (parser));
  gint num_images = json_reader_count_elements (reader);
  if (num_images == -1)
    {
      *error = g_error_copy (json_reader_get_error (reader));
      goto fail2;
    }
  gint count;
  for (count = 0; count < num_images; count++)
    {
      /* Required info */
      const gchar *resource_path;
      /* Optional info; some combinations of these are required though */
      const gchar *original_uri = NULL, *license = NULL, *license_uri = NULL,
        *credit = NULL, *credit_contact = NULL, *copyright_holder = NULL,
        *comment = NULL;
      gint64 copyright_year = -1;
      gboolean permission = FALSE;

      if (!json_reader_read_element (reader, count))
        {
          g_warning ("Could not read element %d of attribution file", count);
          json_reader_end_element (reader);
          continue;
        }

      if (!json_reader_is_object (reader))
        {
          g_warning ("Expected element %d in attribution file to be a dict",
                     count);
          json_reader_end_element (reader);
          continue;
        }

      /* "resource_path" is required */
      if (!json_reader_read_member (reader, "resource_path"))
        {
          g_warning ("Element %d in attribution file must contain a 'resource_path'",
                     count);
          json_reader_end_member (reader);
          json_reader_end_element (reader);
          continue;
        }
      resource_path = json_reader_get_string_value (reader);
      json_reader_end_member (reader);

      /* Read all optional elements */

      gchar **members = json_reader_list_members (reader);

#define READ_MEMBER_IF_PRESENT(member_name, read_func, storage) \
  if (strv_index (members, member_name) != -1) \
    { \
      json_reader_read_member (reader, member_name); \
      storage = json_reader_get_##read_func (reader); \
      json_reader_end_member (reader); \
    }

      READ_MEMBER_IF_PRESENT("uri", string_value, original_uri)
      READ_MEMBER_IF_PRESENT("license", string_value, license)
      READ_MEMBER_IF_PRESENT("license_uri", string_value, license_uri)
      READ_MEMBER_IF_PRESENT("credit", string_value, credit)
      READ_MEMBER_IF_PRESENT("credit_contact", string_value, credit_contact)
      READ_MEMBER_IF_PRESENT("copyright_holder", string_value, copyright_holder)
      READ_MEMBER_IF_PRESENT("comment", string_value, comment)
      READ_MEMBER_IF_PRESENT("copyright_year", int_value, copyright_year)
      READ_MEMBER_IF_PRESENT("permission", boolean_value, permission);

#undef READ_MEMBER_IF_PRESENT

      g_strfreev (members);
      json_reader_end_element (reader);

      /* Validate the data */

      if (license == NULL && license_uri == NULL && credit == NULL &&
          copyright_holder == NULL)
        {
          g_warning ("Image %s must have at least one of the following "
                     "specified: license, license_uri, credit, or "
                     "copyright_holder.", resource_path);
          continue;
        }
      gint license_index = -1;
      if (license != NULL)
        {
          license_index = strv_index (recognized_licenses, license);
          if (license_index == -1)
            {
              g_warning ("License string for image %s not recognized: %s",
                         resource_path, license);
              continue;
            }
          if (license_uri == NULL)
            license_uri = license_uris[license_index];
        }

      /* Populate a row of the model */

      GError *inner_error = NULL;
      GdkPixbuf *pixbuf = gdk_pixbuf_new_from_resource_at_scale (resource_path,
                                                                 -1, ROW_HEIGHT,
                                                                 TRUE,
                                                                 &inner_error);
      if (pixbuf == NULL)
        {
          g_warning ("Not able to load pixbuf from '%s': %s", resource_path,
                     inner_error->message);
          g_clear_error (&inner_error);
          continue;
        }

      GtkTreeIter new_row;
      gtk_list_store_append (priv->model, &new_row);
      gtk_list_store_set (priv->model, &new_row,
                          COLUMN_PIXBUF, pixbuf,
                          COLUMN_ORIGINAL_URI, original_uri,
                          COLUMN_LICENSE, license_index,
                          COLUMN_LICENSE_URI, license_uri,
                          COLUMN_CREDIT, credit,
                          COLUMN_CREDIT_CONTACT, credit_contact,
                          COLUMN_COPYRIGHT_HOLDER, copyright_holder,
                          COLUMN_COPYRIGHT_YEAR, (gint) copyright_year,
                          COLUMN_PERMISSION, permission,
                          COLUMN_COMMENT, comment,
                          -1);
      g_object_unref (pixbuf); /* List store now holds the reference */
    }

  g_object_unref (reader);
  g_object_unref (parser);
  return TRUE;

fail2:
  g_object_unref (reader);
fail:
  g_object_unref (parser);
  return FALSE;
}

static void
eos_attribution_init_initable (GInitableIface *iface,
                               GInterfaceInfo *info)
{
  iface->init = eos_attribution_initable_init;
}

GtkWidget *
eos_attribution_new_sync (GFile        *file,
                          GCancellable *cancellable,
                          GError      **error)
{
  g_return_val_if_fail (G_IS_FILE (file), NULL);
  g_return_val_if_fail (cancellable == NULL || G_IS_CANCELLABLE (cancellable),
                        NULL);
  g_return_val_if_fail (error == NULL || *error == NULL, NULL);

  return GTK_WIDGET (g_initable_new (EOS_TYPE_ATTRIBUTION, cancellable, error,
                                     "file", file,
                                     NULL));
}

GFile *
eos_attribution_get_file (EosAttribution *self)
{
  g_return_val_if_fail (EOS_IS_ATTRIBUTION (self), NULL);

  EosAttributionPrivate *priv = eos_attribution_get_instance_private (self);
  return priv->file;
}
