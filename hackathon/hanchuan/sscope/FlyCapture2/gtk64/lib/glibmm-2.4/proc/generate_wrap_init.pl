#! /bin/perl
#
# tools/generate_wrap_init.pl.  Generated from generate_wrap_init.pl.in by configure.
#

use strict;

my @namespace_whole = (); # list of strings.
my $function_prefix = "";
my $parent_dir = ""; # e.g. gtkmm
my $path = "gtk--";
my $debug = 0;
my @filenames_headers = ();
my %objects = ();
my %exceptions = ();
my %namespaces = (); # hashmap of lists of strings.
my %basenames = ();
my %win32_nowrap = ();
my %deprecated = ();

# Loop through command line arguments, setting variables:
while ($ARGV[0] =~ /^-/)
{
  if ($ARGV[0] =~ /--namespace=(\S+)/)
  {
    push(@namespace_whole, $1);

    if($parent_dir eq "")
      { $parent_dir = lc($1) . "mm"; }
  }
  elsif ($ARGV[0] =~ /--function_prefix=(\S+)/)
  {
    $function_prefix = "$1";
  }
  elsif ($ARGV[0] =~ /--parent_dir=(\S+)/)
  {
    $parent_dir = "$1";
  }
  elsif
  (
    $ARGV[0] =~ /--debug/) {$debug = 1;
  }
  elsif ($ARGV[0] =~ /--path=(\S+)/)
  {
    $path = "$1";
  }
  else
  {
    print "Error: unknown option $ARGV[0]\n";
    exit;
  }

  shift @ARGV;
}



while ($ARGV[0])
{
  if ($debug) {warn "Processing file : $ARGV[0]\n";}

  my $filename = $ARGV[0];
  open FILE, $filename or die "Couldn't open file $ARGV[0] : $!\n";

  # my $file = $ARGV[0];
  # $file =~ s/.*\/([^\/]+)$/$1/;
  # $file =~ s/\.hg//;
  my @tmpnamespace = @namespace_whole;
  my $cppname = "";
  my $cname = "";
  my $ccast = "";
  while (<FILE>)
  {
    if (/CLASS_START\((\w+)\)/) #We need a new way to get the namespace.
    {
      print "generate_wrap_init: namespace found: $1\n";
      push(@tmpnamespace, $1);
    }
    elsif (/_CLASS_GOBJECT\s*\(/) #TODO: There is duplication of code here.
    {
      my $line = $_;
      while ($line !~ /\)/ && ($_ = <FILE>))
      {
        $line .= $_;
      }

      $line =~ s/^.*_CLASS_GOBJECT\s*\(//;
      $line =~ s/\s+//g;
      ($cppname, $cname, $ccast) = split(/,/, $line);

      $objects{$cppname} = $cname;
      @{$namespaces{$cppname}} = ( @tmpnamespace ); #both are lists of strings
      $basenames{$cppname} = lc($ccast);
    }
    elsif (/_CLASS_GTKOBJECT\s*\(/)
    {
      my $line = $_;
      while ($line !~ /\)/ && ($_ = <FILE>))
      {
        $line .= $_;
      }

      $line =~ s/^.*_CLASS_GTKOBJECT\s*\(//;
      $line =~ s/\s+//g;
      ($cppname, $cname, $ccast) = split(/,/, $line);

      #TODO: Remove this hack eventually.
      if( ($cname ne "GtkTree") && ($cname ne "GtkTreeItem") && ($cname ne "GtkText") )
      {
        $objects{$cppname} = $cname;
        @{$namespaces{$cppname}} = ( @tmpnamespace ); #both are lists of strings
        $basenames{$cppname} = lc($ccast);
      }
    }
    elsif (/_WRAP_GERROR\s*\(/) #TODO: There is duplication of code here.
    {
      my $line = $_;
      while ($line !~ /\)/ && ($_ = <FILE>))
      {
        $line .= $_;
      }

      $line =~ s/^.*_WRAP_GERROR\s*\(//;
      $line =~ s/\s+//g;
      $line =~ s/\)//;
      ($cppname, $cname, $ccast) = split(/,/, $line);

      $exceptions{$cppname} = $cname;
      @{$namespaces{$cppname}} = ( @tmpnamespace ); #both are lists of strings
      $basenames{$cppname} = lc($ccast);
    }
    elsif (/_GTKMMPROC_WIN32_NO_WRAP/)
    {
      $win32_nowrap{$cppname} = 1;
    }
    elsif (/_IS_DEPRECATED/)
    {
      $deprecated{$cppname} = 1;
    }
  }

  # Store header filename so that we can #include it later:
  my $filename_header = $filename;
  $filename_header =~ s/.*\/([^\/]+)\.hg/$1.h/;
  push(@filenames_headers, $filename_header);

  shift @ARGV;
  close(FILE);
}

# my $namespace_whole_lower = lc($namespace_whole);

print << "EOF";

#include <glib.h>

// Disable the 'const' function attribute of the get_type() functions.
// GCC would optimize them out because we don't use the return value.
#undef  G_GNUC_CONST
#define G_GNUC_CONST /* empty */

#include <${parent_dir}/wrap_init.h>
#include <glibmm/error.h>
#include <glibmm/object.h>

// #include the widget headers so that we can call the get_type() static methods:

EOF

foreach( @filenames_headers )
{
  print "#include \"" . $_ . "\"\n";
}

print "\n";

# Here we have to be subtle. The gtkmm objects are easy, they all go
# into the Gtk namespace. But in gnomemm, some go into the Gnome
# namespace (most of them), and some into the Gtk one (because the
# corresponding widget is Gtk-prefixed, e.g. GtkTed, GtkDial, etc...

# First, the Gtk namespace

print "extern \"C\"\n";
print "{\n";
print "\n//Declarations of the *_get_type() functions:\n\n";

my $i = 0;
foreach $i (sort keys %objects)
{
  if( $deprecated{$i} eq 1 )
  {
    # The uc(parent_dir) is a bit of a hack. One day it will get it wrong.
    print "#ifndef " . uc($parent_dir) ."_DISABLE_DEPRECATED\n"
  }

  #On Win32, these classes are not compiled:
  if( $win32_nowrap{$i} eq 1 )
  {
    print "#ifndef G_OS_WIN32\n"
  }
  
  print "GType $basenames{$i}_get_type(void);\n";

  if( $win32_nowrap{$i} eq 1 )
  {
    print "#endif //G_OS_WIN32\n"
  }

  if( $deprecated{$i} eq 1 )
  {
    print "#endif // *_DISABLE_DEPRECATED\n"
  }
}

print "\n//Declarations of the *_error_quark() functions:\n\n";

my $i = 0;
foreach $i (sort keys %exceptions)
{
  print "GQuark $basenames{$i}_quark(void);\n";
}

print "} // extern \"C\"\n";
print "\n";

print "\n//Declarations of the *_Class::wrap_new() methods, instead of including all the private headers:\n\n";

my $i = 0;
foreach $i (sort keys %objects)
{
  if( $deprecated{$i} eq 1 )
  {
    # The uc(parent_dir) is a bit of a hack. One day it will get it wrong.
    print "#ifndef " . uc($parent_dir) ."_DISABLE_DEPRECATED\n"
  }

  #On Win32, these classes are not compiled:
  if( $win32_nowrap{$i} eq 1 )
  {
    print "#ifndef G_OS_WIN32\n"
  }
  
  my $namespace_declarations = "";
  my $namespace_close = "";
  foreach ( @{$namespaces{$i}} )
  {
    $namespace_declarations .= "namespace $_ { ";
    $namespace_close .= " }";
  }

  print "${namespace_declarations} class ${i}_Class { public: static Glib::ObjectBase* wrap_new(GObject*); }; ${namespace_close}\n";
  
  if( $win32_nowrap{$i} eq 1 )
  {
    print "#endif //G_OS_WIN32\n"
  }

  if( $deprecated{$i} eq 1 )
  {
    print "#endif // *_DISABLE_DEPRECATED\n"
  }
}

# print "\n//Declarations of the *Error::throw_func() methods:\n\n";
#
# my $i = 0;
# foreach $i (sort keys %exceptions)
# {
#   my $namespace_declarations = "";
#   my $namespace_close = "";
#   foreach ( @{$namespaces{$i}} )
#   {
#     $namespace_declarations .= "namespace $_ { ";
#     $namespace_close .= " }";
#   }
#
#   print "${namespace_declarations} class ${i} { public: static void throw_func(GError*); }; ${namespace_close}\n";
# }

my $namespace_whole_declarations = "";
my $namespace_whole_close = "";
foreach( @namespace_whole )
{
  $namespace_whole_declarations .= "namespace " . $_ ." { ";
  $namespace_whole_close = "} //" . $_ . "\n" . $namespace_whole_close;
}

print "\n";
print "$namespace_whole_declarations\n";
print "\n";
print "void " .$function_prefix . "wrap_init()\n{\n";


# Generate namespace::wrap_init() body
#

print "  // Register Error domains:\n";

foreach $i (sort keys %exceptions)
{
  my $namespace_prefix = "";
  foreach( @{$namespaces{$i}} )
  {
    $namespace_prefix .= $_ ."::";
  }

  print "  Glib::Error::register_domain($basenames{$i}_quark(), &", "${namespace_prefix}${i}::throw_func);\n";
}

print "\n";
print "// Map gtypes to gtkmm wrapper-creation functions:\n";

foreach $i (sort keys %objects)
{
  if( $deprecated{$i} eq 1 )
  {
    # The uc(parent_dir) is a bit of a hack. One day it will get it wrong.
    print "#ifndef " . uc($parent_dir) ."_DISABLE_DEPRECATED\n"
  }

  #On Win32, these classes are not compiled:
  if( $win32_nowrap{$i} eq 1 )
  {
    print "#ifndef G_OS_WIN32\n"
  }
  
  my $namespace_prefix = "";
  foreach( @{$namespaces{$i}} )
  {
    $namespace_prefix .= $_ ."::";
  }

  print "  Glib::wrap_register($basenames{$i}_get_type(), &", "${namespace_prefix}${i}_Class::wrap_new);\n";

  if( $win32_nowrap{$i} eq 1 )
  {
    print "#endif //G_OS_WIN32\n"
  }

  if( $deprecated{$i} eq 1 )
  {
    print "#endif // *_DISABLE_DEPRECATED\n"
  }
}

print "\n";
print "  // Register the gtkmm gtypes:\n";

foreach $i (sort keys %objects)
{
  if( $deprecated{$i} eq 1 )
  {
    # The uc(parent_dir) is a bit of a hack. One day it will get it wrong.
    print "#ifndef " . uc($parent_dir) ."_DISABLE_DEPRECATED\n"
  }

  #On Win32, these classes are not compiled:
  if( $win32_nowrap{$i} eq 1 )
  {
    print "#ifndef G_OS_WIN32\n"
  }
  
  my $namespace_prefix = "";
  foreach( @{$namespaces{$i}} )
  {
    $namespace_prefix .= $_ ."::";
  }

  print "  ${namespace_prefix}${i}::get_type();\n";

  if( $win32_nowrap{$i} eq 1 )
  {
    print "#endif //G_OS_WIN32\n"
  }

  if( $deprecated{$i} eq 1 )
  {
    print "#endif // *_DISABLE_DEPRECATED\n"
  }
}

print << "EOF";

} // wrap_init()

$namespace_whole_close

EOF

exit 0;

