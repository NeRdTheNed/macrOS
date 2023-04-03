import java.io.PrintWriter;
import java.util.logging.Level;
import java.util.logging.LogRecord;
import java.util.logging.Logger;

/**
 * Represents an official macOS SDK constant.
 *
 * @author NeRd
 */
final class MacVersion {

    /**
     * Auto-generated hash code.
     * Truly enlightening Javadoc.
     *
     * @return hash code
     */
    public int hashCode() {
        final int prime = 31;
        int result = 1;
        result = (prime * result) + bugfix;
        result = (prime * result) + major;
        return (prime * result) + minor;
    }

    /**
     * Are the versions the same?.
     *
     * @param obj the MacVersion to compare
     * @return true, if they are the same SDK version
     */
    public boolean equals(final Object obj) {
        if (this == obj) {
            return true;
        }

        if (!(obj instanceof MacVersion)) {
            return false;
        }

        final MacVersion version = (MacVersion) obj;
        return (version.major == major) && (version.minor == minor) && (version.bugfix == bugfix);
    }

    /** Version numbers. */
    private final int major, minor, bugfix;

    /** Cached string representations of the version */
    public final String asVername, asTriple;

    /**
     * Creates a MacVersion, and caches
     * commonly used String representations of it.
     *
     * @param major the major version
     * @param minor the minor version
     * @param bugfix the bugfix version
     */
    public MacVersion(final int major, final int minor, final int bugfix) {
        this.major = major;
        this.minor = minor;
        this.bugfix = bugfix;
        asVername = toVername();
        asTriple = toTriple();
    }

    /**
     * Creates a String representation of this version
     * as the name of a macOS SDK constant definition
     * (e.g MAC_OS_X_VERSION_10_0).
     *
     * @return this version as the name of a macOS SDK constant definition
     */
    private String toVername() {
        final StringBuffer buffer = new StringBuffer()
        .append(major > 10 ? "MAC_OS_VERSION_" : "MAC_OS_X_VERSION_")
        .append(major)
        .append("_")
        .append(minor);

        if (bugfix != 0) {
            buffer.append("_")
            .append(bugfix);
        }

        return buffer.toString();
    }

    /**
     * Creates a String representation of this version
     * as an underscore separated triple
     * (e.g 10_0_0).
     *
     * @return this version as an underscore separated triple
     */
    private String toTriple() {
        return new StringBuffer()
               .append(major)
               .append("_")
               .append(minor)
               .append("_")
               .append(bugfix)
               .toString();
    }

    /**
     * Creates a String representation of this version
     * as the value of a macOS SDK constant definition
     * (e.g 1000).
     *
     * @return this version as the value of a macOS SDK constant definition
     */
    public String toVernum() {
        final StringBuffer buffer = new StringBuffer().append(major);

        /* Pad with zeros when post macOS 11 */

        if ((major > 10) && (minor < 10)) {
            buffer.append(0);
        }

        buffer.append(minor);

        /* Pad with zeros when post macOS 10.10 */

        if (((major > 10) || ((major == 10) && (minor >= 10))) && (bugfix < 10)) {
            buffer.append(0);
        }

        return buffer.append(bugfix).toString();
    }
}

/**
 * I may have gone too far in a few places.
 * <br>
 * This code generates macOS version availability ranges,
 * because different macOS SDKs have different version ranges defined.
 * <br>
 * Generated output includes:
 * <ul>
 *   <li>Every official macOS SDK version constant</li>
 *   <li>
 *     Compile time defines to determine which versions of macOS
 *     your program can / will run on based on:
 *     <ul>
 *       <li>Deployment target</li>
 *       <li>Architecture</li>
 *     </ul>
 *   </li>
 * </ul>
 *
 * @author NeRd
 */
final class Avail {
    /** What value a macro is defined as. */
    private static final String DEFINED_VAL_TRUE = "";

    /** Prefix for NEVER_HAVE defines. */
    private static final String NEVER_HAVE = "macrOS_NEVER_HAVE_";

    /** Prefix for ALWAYS_HAVE defines. */
    private static final String ALWAYS_HAVE = "macrOS_ALWAYS_HAVE_";

    /** All official macOS SDK version constants. */
    private static final MacVersion[] MAC_VERSIONS = {
        new MacVersion(10, 0, 0),
        new MacVersion(10, 1, 0),
        new MacVersion(10, 2, 0),
        new MacVersion(10, 3, 0),
        new MacVersion(10, 4, 0),
        new MacVersion(10, 5, 0),
        new MacVersion(10, 6, 0),
        new MacVersion(10, 7, 0),
        new MacVersion(10, 8, 0),
        new MacVersion(10, 9, 0),
        new MacVersion(10, 10, 0),
        new MacVersion(10, 10, 2),
        new MacVersion(10, 10, 3),
        new MacVersion(10, 11, 0),
        new MacVersion(10, 11, 2),
        new MacVersion(10, 11, 3),
        new MacVersion(10, 11, 4),
        new MacVersion(10, 12, 0),
        new MacVersion(10, 12, 1),
        new MacVersion(10, 12, 2),
        new MacVersion(10, 12, 4),
        new MacVersion(10, 13, 0),
        new MacVersion(10, 13, 1),
        new MacVersion(10, 13, 2),
        new MacVersion(10, 13, 4),
        new MacVersion(10, 14, 0),
        new MacVersion(10, 14, 1),
        new MacVersion(10, 14, 4),
        new MacVersion(10, 14, 6),
        new MacVersion(10, 15, 0),
        new MacVersion(10, 15, 1),
        new MacVersion(10, 16, 0),
        new MacVersion(11, 0, 0),
        new MacVersion(11, 1, 0),
        new MacVersion(11, 3, 0),
        new MacVersion(12, 0, 0),
        new MacVersion(13, 0, 0),
        new MacVersion(13, 1, 0),
        new MacVersion(13, 3, 0)
    };

    /**
     * Given a ifdef condition, this method appends a list of version range exclusions from start to end.
     * Null represents the earliest or latest SDK version for start and end respectively.
     *
     * @param buffer the StringBuffer to append to
     * @param cond the ifdef condition
     * @param start the starting version, or null for the earliest SDK version
     * @param end the end version, or null for the latest SDK version
     */
    private static void makeRangeBetween(final StringBuffer buffer, final String cond, final MacVersion start, final MacVersion end) {
        /* This makes no logical sense to do, as this would unconditionally exclude every version of macOS */
        if ((start == null) && (end == null)) {
            Logger.getLogger("Avail").log(Level.WARNING, "makeRangeBetween was called for condition {0} with two null versions", cond);
            return;
        }

        final int length = MAC_VERSIONS.length;
        int startIndex = -1, endIndex = -1;

        for (int i = 0; i < length; i++) {
            if ((start != null) && (startIndex == -1) && (MAC_VERSIONS[i].equals(start))) {
                startIndex = i;

                if (end == null) {
                    break;
                }

                continue;
            }

            if ((end != null) && (MAC_VERSIONS[i].equals(end))) {
                endIndex = i;
                break;
            }
        }

        if (startIndex == -1) {
            startIndex = 0;
        }

        if (endIndex == -1) {
            endIndex = length;
        }

        buffer.append("#if ").append(cond).append("\n");

        for (int i = startIndex; i < endIndex; i++) {
            buffer.append("#ifndef " + NEVER_HAVE + MAC_VERSIONS[i].asTriple + "\n")
            .append("#define " + NEVER_HAVE + MAC_VERSIONS[i].asTriple + DEFINED_VAL_TRUE + "\n")
            .append("#endif" + "\n");
        }

        buffer.append("#endif" + "\n\n");
    }

    /**
     * Given a ifdef condition, this method appends a list of version range exclusions from start until the latest SDK version.
     * Convenience method for calling makeRangeBetween(buffer, cond, start, null);
     *
     * @param buffer the StringBuffer to append to
     * @param cond the ifdef condition
     * @param start the starting version
     */
    private static void makeRangeFromVersionOnwards(final StringBuffer buffer, final String cond, final MacVersion start) {
        makeRangeBetween(buffer, cond, start, null);
    }

    /**
     * Given a ifdef condition, this method appends a list of version range exclusions from the earliest SDK version until end.
     * Convenience method for calling makeRangeBetween(buffer, cond, null, end);
     *
     * @param buffer the StringBuffer to append to
     * @param cond the ifdef condition
     * @param end the end version
     */
    private static void makeRangeUntilVersion(final StringBuffer buffer, final String cond, final MacVersion end) {
        makeRangeBetween(buffer, cond, null, end);
    }

    public static void main(final String[] args) {
        final Logger log = Logger.getLogger("Avail");
        log.log(Level.INFO, "Avail: Generates macOS SDK constants, and defines version ranges for compilation.");
        String outputPath = null;

        if (args.length > 0) {
            if ((args.length % 2) == 0) {
                for (int i = 0; i < args.length; i += 2) {
                    try {
                        if ("-outputPath".equals(args[i])) {
                            outputPath = args[i + 1];
                        } else {
                            log.log(Level.SEVERE, "Invalid command line parameter {0} provided", args[i]);
                            System.exit(1);
                        }
                    } catch (final Exception e) {
                        final LogRecord logRecord = new LogRecord(Level.SEVERE, "The command line parameter for {0} was not able to be parsed");
                        logRecord.setParameters(new Object[] { args[i] });
                        logRecord.setThrown(e);
                        log.log(logRecord);
                        System.exit(1);
                    }
                }
            } else {
                log.log(Level.SEVERE, "An incorrect amount of command line parameters was provided");
                System.exit(1);
            }
        }

        if (outputPath == null) {
            outputPath = "./GeneratedAvail.h";
        }

        final int length = MAC_VERSIONS.length;
        /* Start generated source */
        final StringBuffer buffer = new StringBuffer()
        .append("#ifndef macrOS_EXTENDED_MAC_OS_GENERATED_AVAIL_H\n#define macrOS_EXTENDED_MAC_OS_GENERATED_AVAIL_H\n\n");

        /* Define version numbers */
        for (int i = 0; i < length; i++) {
            buffer.append("#ifndef " + MAC_VERSIONS[i].asVername + "\n")
            .append("#define " + MAC_VERSIONS[i].asVername + " " + MAC_VERSIONS[i].toVernum() + "\n")
            .append("#endif" + "\n");
        }

        /* Define ALWAYS_HAVE constants */
        buffer.append("\n")
        .append("#if defined(" + MAC_VERSIONS[0].asVername + ") && MAC_OS_X_VERSION_MIN_REQUIRED >= " + MAC_VERSIONS[0].asVername + "\n")
        .append("#define " + ALWAYS_HAVE + MAC_VERSIONS[0].asTriple + DEFINED_VAL_TRUE + "\n")
        .append("#endif" + "\n");

        for (int i = 1; i < length; i++) {
            buffer.append("#if defined(" + MAC_VERSIONS[i].asVername + ") && MAC_OS_X_VERSION_MIN_REQUIRED >= " + MAC_VERSIONS[i].asVername + "\n")
            .append("#define " + NEVER_HAVE + MAC_VERSIONS[i - 1].asTriple + DEFINED_VAL_TRUE + "\n")
            .append("#define " + ALWAYS_HAVE + MAC_VERSIONS[i].asTriple + DEFINED_VAL_TRUE + "\n")
            .append("#endif" + "\n");
        }

        buffer.append("\n");
        /* Define NEVER_HAVE constants for archs */
        /* PowerPC */
        /* - Post PowerPC systems */
        makeRangeFromVersionOnwards(buffer, "defined(__ppc64__) || defined(__ppc__)", new MacVersion(10, 7, 0));
        /* Intel */
        /* - Pre Intel systems */
        makeRangeUntilVersion(buffer, "defined(__i386__) || defined(__x86_64__)", new MacVersion(10, 4, 0));
        /* - Pre Intel 64 bit systems */
        makeRangeBetween(buffer, "defined(__x86_64__)", new MacVersion(10, 4, 0), new MacVersion(10, 5, 0));
        /* - Post Intel 32 bit systems */
        makeRangeFromVersionOnwards(buffer, "defined(__i386__)", new MacVersion(10, 15, 0));
        /* ARM64 */
        /* - Pre ARM64 systems */
        makeRangeUntilVersion(buffer, "defined(__aarch64__)", new MacVersion(11, 0, 0));
        /* End generated source */
        buffer.append("#endif /* macrOS_EXTENDED_MAC_OS_GENERATED_AVAIL_H */\n");
        /* Output */
        PrintWriter output = null;
        boolean didError = false;

        try {
            output = new PrintWriter(outputPath);
            output.print(buffer.toString());
        } catch (final Exception e) {
            didError = true;
            log.log(Level.SEVERE, "Exception was thrown while writing generated header to file", e);
        } finally {
            if (output != null) {
                output.close();
            }
        }

        if (didError) {
            log.log(Level.INFO, "Error when running header generator program, please check if generated output is correct");
            System.exit(1);
        } else {
            log.log(Level.INFO, "Successfully generated macOS SDK constants and version ranges");
        }
    }
}
