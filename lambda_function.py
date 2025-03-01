import boto3
import json

# Initialize SNS client in the correct region
sns_client = boto3.client("sns", region_name="us-east-1")

# ARN of your SNS Topic
TOPIC_ARN = "arn:aws:sns:us-east-1:440777116651:bing_bing_alert"  # this is where you insert your sns topic ARN

def lambda_handler(event, context):
    message = "" # define the message you wish to send 

    try:
        # Publish message to SNS Topic (not directly to phone number)
        response = sns_client.publish(
            TopicArn=TOPIC_ARN,
            Message=message
        )

        return {
            "statusCode": 200,
            "body": json.dumps(response)
        }
    except Exception as e:
        return {
            "statusCode": 500,
            "body": json.dumps({"error": str(e)})
        }
