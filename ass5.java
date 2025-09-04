import java.util.Scanner;

public class ass5 {

    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);

        // CIDR base input
        String cidr = "192.168.31.0/26";
        String[] parts = cidr.split("/");
        String ipAddress = parts[0];
        int prefix = Integer.parseInt(parts[1]);

        System.out.print("Enter number of subnets: ");
        int numSubnets = scanner.nextInt();

        // Calculate bits needed for subnetting
        int bitsForSubnetting = (int) Math.ceil(Math.log(numSubnets) / Math.log(2));
        int newPrefix = prefix + bitsForSubnetting;

        if (newPrefix > 32) {
            System.out.println("Error: Too many subnets requested for this CIDR block.");
            return;
        }

        int totalSubnets = (int) Math.pow(2, bitsForSubnetting);
        int totalHostsPerSubnet = (int) Math.pow(2, 32 - newPrefix);

        System.out.println("\nSubnetting " + cidr + " into " + totalSubnets + " subnets with prefix /" + newPrefix + ":");

        int baseIP = ipToInt(ipAddress);

        for (int i = 0; i < totalSubnets; i++) {
            int networkIP = baseIP + (i * totalHostsPerSubnet);
            int broadcastIP = networkIP + totalHostsPerSubnet - 1;

            System.out.println("\nSubnet " + (i + 1) + ":");
            System.out.println("  Decimal : " + intToIp(networkIP) + " to " + intToIp(broadcastIP));
            System.out.println("  Binary  : " + ipToBinary(networkIP) + " to " + ipToBinary(broadcastIP));
        }
    }

    // Convert dotted decimal IP to integer
    public static int ipToInt(String ip) {
        String[] parts = ip.split("\\.");
        int result = 0;
        for (String part : parts) {
            result = (result << 8) | Integer.parseInt(part);
        }
        return result;
    }

    // Convert integer IP to dotted decimal
    public static String intToIp(int ip) {
        return String.format("%d.%d.%d.%d",
                (ip >> 24) & 0xFF,
                (ip >> 16) & 0xFF,
                (ip >> 8) & 0xFF,
                ip & 0xFF);
    }

    // Convert integer IP to binary format (8 bits per octet)
    public static String ipToBinary(int ip) {
        return String.format("%8s.%8s.%8s.%8s",
                Integer.toBinaryString((ip >> 24) & 0xFF),
                Integer.toBinaryString((ip >> 16) & 0xFF),
                Integer.toBinaryString((ip >> 8) & 0xFF),
                Integer.toBinaryString(ip & 0xFF)).replace(' ', '0');
    }
}
