#include <glib.h>

#include <wocky/wocky-caps-hash.h>
#include <wocky/wocky-stanza.h>

#include "wocky-test-helper.h"

static gboolean
check_hash (WockyStanza *stanza,
  const gchar *expected)
{
  gchar *hash;

  hash = wocky_caps_hash_compute_from_node (
      wocky_stanza_get_top_node (stanza));
  g_assert_cmpstr (hash, ==, expected);
  g_object_unref (stanza);
  g_free (hash);
  return TRUE;
}

static void
test_simple (void)
{
  /* Simple example from XEP-0115 */
  WockyStanza *stanza = wocky_stanza_build (WOCKY_STANZA_TYPE_IQ,
      WOCKY_STANZA_SUB_TYPE_RESULT, NULL, NULL,
      '(', "identity",
        '@', "category", "client",
        '@', "name", "Exodus 0.9.1",
        '@', "type", "pc",
      ')',
      '(', "feature",
          '@', "var", "http://jabber.org/protocol/disco#info", ')',
      '(', "feature",
          '@', "var", "http://jabber.org/protocol/disco#items", ')',
      '(', "feature", '@', "var", "http://jabber.org/protocol/muc", ')',
      '(', "feature", '@', "var", "http://jabber.org/protocol/caps", ')',
      NULL);

  check_hash (stanza, "QgayPKawpkPSDYmwT/WM94uAlu0=");
}

static void
test_complex (void)
{
  /* Complex example from XEP-0115 */
  WockyStanza *stanza = wocky_stanza_build (WOCKY_STANZA_TYPE_IQ,
      WOCKY_STANZA_SUB_TYPE_NONE, NULL, "badger",
      '(', "identity",
        '@', "category", "client",
        '@', "name", "Psi 0.11",
        '@', "type", "pc",
        '#', "en",
      ')',
      '(', "identity",
        '@', "category", "client",
        '@', "name", "Ψ 0.11",
        '@', "type", "pc",
        '#', "el",
      ')',
      '(', "feature", '@', "var", "http://jabber.org/protocol/disco#info", ')',
      '(', "feature", '@', "var", "http://jabber.org/protocol/disco#items", ')',
      '(', "feature", '@', "var", "http://jabber.org/protocol/muc", ')',
      '(', "feature", '@', "var", "http://jabber.org/protocol/caps", ')',
      '(', "x",
        ':', "jabber:x:data",
        '@', "type", "result",
        '(', "field",
          '@', "var", "FORM_TYPE",
          '@', "type", "hidden",
          '(', "value", '$', "urn:xmpp:dataforms:softwareinfo", ')',
        ')',
        '(', "field",
          '@', "var", "ip_version",
          '(', "value", '$', "ipv4", ')',
          '(', "value", '$', "ipv6", ')',
        ')',
        '(', "field",
          '@', "var", "os",
          '(', "value", '$', "Mac", ')',
        ')',
        '(', "field",
          '@', "var", "os_version",
          '(', "value", '$', "10.5.1", ')',
        ')',
        '(', "field",
          '@', "var", "software",
          '(', "value", '$', "Psi", ')',
        ')',
        '(', "field",
          '@', "var", "software_version",
          '(', "value", '$', "0.11", ')',
        ')',
      ')',
      NULL);

  check_hash (stanza, "q07IKJEyjvHSyhy//CH0CxmKi8w=");
}

int
main (int argc, char **argv)
{
  int result;

  test_init (argc, argv);
  g_test_add_func ("/caps-hash/simple", test_simple);
  g_test_add_func ("/caps-hash/complex", test_complex);

  result = g_test_run ();
  test_deinit ();

  return result;
}
